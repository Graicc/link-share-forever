{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc
    openssl
    
    # C man pages
    glibcInfo
    man-pages

    # make
    gnumake
 ];
}
