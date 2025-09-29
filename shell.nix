let
  unstable = import <nixos-unstable> {};
in
{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShellNoCC {
  buildInputs = with unstable; [
    gcc15
    libxcrypt
    curl
    gdb
    
    # C man pages
    glibcInfo
    man-pages

    # make
    gnumake

    sqlite
 ];
}
