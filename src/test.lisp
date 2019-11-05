


(profile :core-include
(include "src/core.lisp")
)

;(profile :whole-execution
(profile :simplex-include 
(include "src/simplex-noise.lisp")
)

(profile :simplex-setup
(seed)
(def simplex (simplex-noise 64 0.9 (rand 99999)))
)

(profile :simplex-test

	(let (t (map #(simplex-noise-value simplex (rand 999) (rand 999)) (range 10))
	      miiin (min t)
		  maaax (max t))
		;(write "TEST " t)
		(write "min" miiin)
		(write "max" maaax)
	)

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

	(let (ts 32 ;16
		  scale 0.95
		  plineMul 0.5
		  nlineMul (* -1 plineMul)
		  nx (/ canvas-w ts)
		  ny (/ canvas-h ts))
		(doseq (j (range ny))
			(doseq (i (range nx))
				(let (cx  (+ (* ts i) (* ts 0.5))
					  cy  (+ (* ts j) (* ts 0.5))
					  simplex-val (simplex-noise-value simplex (* i scale) (* j scale))
					  n   (* 40 (+ 0.5 simplex-val)))
					;(write "simplex-val " simplex-val)
					(doseq (k (range n))
						(line c
								(point (+ cx (rand (* nlineMul ts) (* plineMul ts))) (+ cy (rand (* nlineMul ts) (* plineMul ts))))
								(point (+ cx (rand (* nlineMul ts) (* plineMul ts))) (+ cy (rand (* nlineMul ts) (* plineMul ts))))))))))


)

(profile :drawing-clean
(surface-clean s c)
)
)
