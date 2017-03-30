#pragma once

#include <string>
#include <memory>
#include <vector>
#include <algorithm>

#include "colour.hh"
#include "format.hh"

namespace Misc
{
    /*! The fancy string module allows for combination of text elements
     * and style elements into a single consistent interface. This endows
     * the normal string interface with possibilities for meta information
     * like colour, style, tags and so on. The combined data structure is
     * able to compute the total length of the string as it will appear on
     * the screen.
     */
    namespace fancy {
        using std::size_t;
        using colour::Colour;

        /*! Abstract base class for elements in a fancy string
         */
        class Element {
        public:
            virtual size_t length() const = 0;
            virtual std::string str() const = 0;
            virtual std::unique_ptr<Element> copy() const = 0;
            virtual ~Element() noexcept {}
        };

        /*! std::unique_ptr to Element */
        using ptr = std::unique_ptr<Element>;

        /*! ptr can be sent to std::ostream */
        inline std::ostream &operator<<(std::ostream &out, ptr const &e) {
            return out << e->str();
        }

        /*! or by reference */
        inline std::ostream &operator<<(std::ostream &out, Element const &e) {
            return out << e.str();
        }

        /*! Style element, a container for style information that does
         * not add to the total length of the string
         */
        class Style : public Element {
            std::string const content;

        public:
            Style(std::string const &content) :
                    content(content) { }

            ptr copy() const { return ptr(new Style(*this)); }
            size_t length() const { return 0; }
            std::string str() const { return content; }
        };

        /*! String element, a container for text that does add to the
         * total length of the string.
         */
        class String : public Element {
            std::string const content;

        public:
            String(std::string const &content) :
                    content(content) { }

            ptr copy() const { return ptr(new String(*this)); }
            size_t length() const { return content.length(); }
            std::string str() const { return content; }
        };

        /*! Holds a std::vector of std::unique_ptr to Element. The MultiString
         * container gives the total length of the items contained and you can
         * send it to a std::ostream.
         */
        class MultiString : public Element {
            std::vector<ptr> data;

        public:
            MultiString() {}

            MultiString(MultiString const &m)
            {
                for (ptr const &p : m.data)
                    push_back(p->copy());
            }

            ptr copy() const
            {
                return ptr(new MultiString(*this));
            }

            static std::unique_ptr<MultiString> create()
            {
                return std::make_unique<MultiString>();
            }

            void push_back(ptr &&e) {
                data.push_back(std::move(e));
            }

            size_t length() const {
                return std::accumulate(data.begin(), data.end(), unsigned(0),
                                       [](unsigned c, ptr const &e) {
                                           return c + e->length();
                                       });
            }

            std::string str() const {
                return std::accumulate(data.begin(), data.end(), std::string(),
                                       [](std::string s, ptr const &e) {
                                           return s + e->str();
                                       });
            }

            MultiString &operator<<(std::string const &s) {
                data.emplace_back(new String(s));
                return *this;
            }

            MultiString &operator<<(ptr p) {
                data.push_back(std::move(p));
                return *this;
            }
        };

        /* MultiString composers */
        inline void compose_on(MultiString &m) { }

        template<typename ...Args>
        inline void compose_on(MultiString &m, ptr &&e, Args &&...args) {
            m.push_back(std::move(e));
            compose_on(m, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        inline void compose_on(MultiString &m, Element const &e, Args &&...args) {
            m.push_back(std::move(e.copy()));
            compose_on(m, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        inline void compose_on(MultiString &m, std::string const &s, Args &&...args) {
            m.push_back(std::make_unique<String>(s));
            compose_on(m, std::forward<Args>(args)...);
        }

        /*! compose multiple objects to a MultiString. */
        template<typename ...Args>
        inline ptr compose(Args &&...args) {
            auto m = std::make_unique<MultiString>();
            compose_on(*m, std::forward<Args>(args)...);
            return std::move(m);
        }

        template <typename R, typename Fn>
        ptr intersperse_map(R begin, R end, Fn func, ptr sep)
        {
            auto result = MultiString::create();
            result->push_back(func(*begin));
            ++begin;
            while (begin != end)
            {
                result->push_back(sep->copy());
                result->push_back(func(*begin));
                ++begin;
            }

            return std::move(result);
        }
    }
}

