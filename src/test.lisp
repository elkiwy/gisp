

(include "src/core.lisp")

(def canvas-w 256)
(def canvas-h 256)




(defn reg-shape (context center side-n size)
	(let (cx (get center :x)
		  cy (get center :y))
		(line context (point (+ cx   0) (+ cy   0)) (point (+ cx 100) (+ cy   0)))
		(line context (point (+ cx 100) (+ cy   0)) (point (+ cx 100) (+ cy 100)))
		(line context (point (+ cx 100) (+ cy 100)) (point (+ cx   0) (+ cy 100)))
		(line context (point (+ cx   0) (+ cy 100)) (point (+ cx   0) (+ cy   0)))
	    )
	)


(def s (make-surface "test.svg" canvas-w canvas-h))
(surface-status s)
(def c (make-context s))

(reg-shape c (point 10 10) 4 100)

(surface-clean s c)
