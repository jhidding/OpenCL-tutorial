
LaTeX style guide
=================

- Indent size 2, no tabs.
- Listen to chktex.
- Write paragraphs on a single line, use an editor with line-break mode.
  In Vim, I use the settings `linebreak` and `breakindent`, as well as
  remapping cursor keys to their soft-break alternatives:

```vim
set expandtab
set ts=4
set sw=4
set linebreak
set breakindent
 
noremap <buffer> <silent> <Up>          gk
noremap <buffer> <silent> <Down>        gj
noremap <buffer> <silent> <Home>        g<Home>
noremap <buffer> <silent> <End>         g<End>

inoremap <buffer> <silent> <Up>         <C-o>gk
inoremap <buffer> <silent> <Down>       <C-o>gj
inoremap <buffer> <silent> <Home>       <C-o>g<Home>
inoremap <buffer> <silent> <End>        <C-o>g<End>
```

- Use `acronym` package for acronyms, even obvious ones like CPU,
  `\acs{CPU}` forces the (short) abbreviated version of the acronym,
  plural is achieved with `\acps{CPU}`.

- Sources are included using `minted`. This uses the Python `pygments`
  package to do syntax highlighting. Build with `xelatex -shell-escape`.

