;(include "src/simplex-noise.lisp")
;(write "Simplex included")



(include "src/core.lisp")
(write "Core included")

(def canvas-w 256)
(def canvas-h 256)

(def s (make-surface "test.svg" canvas-w canvas-h))
(def c (make-context s))

(doseq (i (range 10))
	(reg-shape c (point 128 128) 4 100 (* i 10)))

(surface-clean s c)
