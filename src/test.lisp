(profile :core-include
(include "src/core.lisp")
)

(profile :whole-execution
(profile :simplex-include 
(include "src/simplex-noise.lisp")
)


(profile :simplex-setup
(seed)
(def simplex (simplex-noise 64 0.5 (rand 99999)))
)

(profile :simplex-test
(simplex-noise-value simplex 1 1)
(simplex-noise-value simplex 10 1)
(simplex-noise-value simplex 1 3)
(simplex-noise-value simplex 3 3)
(simplex-noise-value simplex 2.5 2.5)
)

(profile :drawing-setup
(def canvas-w 512)
(def canvas-h 512)
(def s (make-surface "test.svg" canvas-w canvas-h))
(def c (make-context s))
)


(profile :drawing-test

;(doseq (i (range 5000))
	;(reg-shape c (point 128 128) 4 100 (* i 10)))

	(let (ts 16
		  scale 0.95
		  plineMul 0.5
		  nlineMul (* -1 plineMul)
		  nx (/ canvas-w ts)
		  ny (/ canvas-h ts))
		(doseq (j (range ny))
			(doseq (i (range nx))
				(let (cx  (+ (* ts i) (* ts 0.5))
					  cy  (+ (* ts j) (* ts 0.5))
					  n   (* 40 (+ 0.5 (simplex-noise-value simplex (* i scale) (* j scale)))))
					(doseq (k (range n))
						(line c
								(point (+ cx (rand (* nlineMul ts) (* plineMul ts))) (+ cy (rand (* nlineMul ts) (* plineMul ts))))
								(point (+ cx (rand (* nlineMul ts) (* plineMul ts))) (+ cy (rand (* nlineMul ts) (* plineMul ts))))))))))


)

(profile :drawing-clean
(surface-clean s c)
)
)
