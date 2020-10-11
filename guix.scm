;; Guix CoC-free
;; Copyright (C) 2019 Ingo Ruhnke <grumbel@gmail.com>
;;
;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

(set! %load-path
  (cons* "/ipfs/QmdHSWX34MXZeAgMuDuFwrYzSPf4fknX7E7cYJ7HDxaLrZ/guix-cocfree_0.0.0-62-g3b27118"
         %load-path))

(use-modules ((guix licenses) #:prefix license:)
             (gnu packages compression)
             (gnu packages gcc)
             (gnu packages gl)
             (gnu packages pkg-config)
             (gnu packages pretty-print)
             (gnu packages serialization)
             (gnu packages xorg)
             (guix build-system cmake)
             (guix git-download)
             (guix packages)
             (guix utils)
             (guix-cocfree utils))

(define %source-dir (dirname (current-filename)))

(define-public construo
  (package
   (name "construo")
   (version (version-from-source %source-dir))
   (source (source-from-source %source-dir #:version version))
   (build-system cmake-build-system)
   (arguments
    ;; project doesn't have tests
    `(#:tests? #f))
   (native-inputs
    `(("pkg-config" ,pkg-config)
      ("gcc-10" ,gcc-10)))
   (inputs
    `(("freeglut" ,freeglut)
      ("libx11" ,libx11)
      ("fmt" ,fmt)
      ("jsoncpp" ,jsoncpp)
      ("zlib" ,zlib)))
   (synopsis (synopsis-from-source %source-dir))
   (description (description-from-source %source-dir))
   (home-page (homepage-from-source %source-dir))
   (license license:gpl3+)))

construo

;; EOF ;;
