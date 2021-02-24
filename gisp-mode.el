;;; gisp-mode.el --- sample major mode for editing Gisp. -*- coding: utf-8; lexical-binding: t; -*-

;; Copyright © 2019, by Stefano Bertoli

;; Author: Stefano Bertoli ( elkiwydev@gmail.com )
;; Version: 0.0.1
;; Created: 26 Nov 2019
;; Keywords: languages
;; Homepage: https://github.com/elkiwy/gisp

;; This file is not part of GNU Emacs.

;;; License:

;; You can redistribute this program and/or modify it under the terms of the GNU General Public License version 2.

;;; Code:



;; create the list for font-lock.
;; each category of keyword is given a particular face

(setq gisp-font-lock-keywords
      (let* (
            ;; define several category of keywords
			(x-keywords '(
					"if" "lambda" "def" "defn" "progn"
					"let" "doseq"))

			(x-functions '(
					;Main.c
					"int" "null" "list" "vector" "hashmap"
					"count" "assoc" "/" "*" "+"
					"-" "bit-and" "mod" "pow" "sqrt"
					"log" "log10" "atan2" "rand" "seed"
					"=" "<" ">" "sin" "cos"
					"dsin" "dcos" "deg" "rad" "floor"
					"ceil" "min" "max" "range" "reverse"
					"concat" "first" "last" "make-surface" "make-context"
					"surface-status" "surface-clean" "line" "surface-to-png" "str"
					"include" "read" "write" "null?" "symbol?"
					"pair?" "eq?" "cons" "cdr" "car"
					"get" "map" "mapv" "apply" "quote"
					"reduce" "profile"

					;Core.gisp
                    "point" "vec" "point-distance" "point-angle" "point-between" "point-move-by-vector" "reg-shape"

					;Simplex-noise.gisp
				    "swap-random" "N-times" "simplex-noise-octave" "fast-floor" "dot" "contribution" "noise" "simplex-noise" "simplex-noise-value"))

            (x-types '("float" "integer" "key" "list" "rotation" "string" "vector"))
            (x-constants '("ACTIVE" "AGENT" "ALL_SIDES" "ATTACH_BACK"))
            (x-events '("at_rot_target" "at_target" "attach"))


            ;; generate regex string for each category of keywords
            (x-keywords-regexp (regexp-opt x-keywords 'words))
            (x-types-regexp (regexp-opt x-types 'words))
            (x-constants-regexp (regexp-opt x-constants 'words))
            (x-events-regexp (regexp-opt x-events 'words))
			(x-functions-regexp (regexp-opt x-functions 'words))
	    )


        `(
          (,x-types-regexp . font-lock-type-face)

          ;(,x-constants-regexp . font-lock-constant-face)

		  ("\\(-[[:digit:]]\\)" . font-lock-constant-face)
		  ("\\([[:digit:]].[[:digit:]]\\)" . font-lock-constant-face)
		  ("\\([[:digit:]]\\)" . font-lock-constant-face)
		  ("\\([*|-|+|/|=|<|>]\\)" . font-lock-function-name-face)



          (,x-events-regexp . font-lock-builtin-face)
          (,x-functions-regexp . font-lock-function-name-face)
          (,x-keywords-regexp . font-lock-keyword-face)
          ;; note: order above matters, because once colored, that part won't change.
          ;; in general, put longer words first
          )))

;;;###autoload
(define-derived-mode gisp-mode lisp-mode
	"Major mode for editing gisp."
	;; code for syntax highlighting
	(setq font-lock-defaults '((gisp-font-lock-keywords))))

(add-to-list 'auto-mode-alist '("\\.gisp\\'" . gisp-mode))

;; add the mode to the `features' list
(provide 'gisp-mode)

;;; gisp-mode.el ends here
