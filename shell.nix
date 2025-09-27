let
  unstable = import <nixos-unstable> {};
in
{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShellNoCC {
  buildInputs = with unstable; [
    gcc15
    openssl
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
