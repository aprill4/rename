set number
set mouse=a
set tabstop=4
set shiftwidth=4
set showmode
filetype indent on
set autoindent
set showcmd
set showmatch
set hlsearch
set incsearch
set smartcase
set noswapfile
set nobackup
set undofile
set noerrorbells
inoremap jk <Esc>

call plug#begin()

Plug 'jonathanfilip/vim-lucius'
Plug 'vim-scripts/twilight256.vim'
Plug 'jiangmiao/auto-pairs'
Plug 'ycm-core/YouCompleteMe'
call plug#end()

colorscheme lucius
set background=dark
