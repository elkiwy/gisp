(profile :core-include
(include "src/core.lisp")
)

(profile :simplex-include 
(include "src/simplex-noise.lisp")
)


(profile :simplex-test
(def simplex (simplex-noise 64 0.5 0))
(write "VAL " (simplex-noise-value simplex 1 1))
(write "VAL " (simplex-noise-value simplex 10 1))
(write "VAL " (simplex-noise-value simplex 1 3))
(write "VAL " (simplex-noise-value simplex 3 3))
(write "VAL " (simplex-noise-value simplex 2.5 2.5))
)

(profile :drawing-setup
(def canvas-w 256)
(def canvas-h 256)
(def s (make-surface "test.svg" canvas-w canvas-h))
(def c (make-context s))
)

(profile :drawing-test
(doseq (i (range 5000))
	(reg-shape c (point 128 128) 4 100 (* i 10)))
)

(profile :drawing-clean
(surface-clean s c)
)
