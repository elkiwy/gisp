
(include "src/simplex-noise.lisp")
(write "Simplex included")


(def simplex (simplex-noise 64 0.5 0))
;(write "simplex " simplex)
(write "VAL " (simplex-noise-value simplex 1 1))
(write "VAL " (simplex-noise-value simplex 10 1))
(write "VAL " (simplex-noise-value simplex 1 3))
(write "VAL " (simplex-noise-value simplex 3 3))
(write "VAL " (simplex-noise-value simplex 2.5 2.5))




;(include "src/core.lisp")
;(write "Core included")
;
;(def canvas-w 256)
;(def canvas-h 256)
;
;(def s (make-surface "test.svg" canvas-w canvas-h))
;(def c (make-context s))
;
;(doseq (i (range 10))
	;(reg-shape c (point 128 128) 4 100 (* i 10)))
;
;(surface-clean s c)
