

(defn point (x y)
	(hashmap :x x :y y))

(def PI 3.14159265)

(defn reg-shape (cont center side-n size)
	(let (cx (get center :x)
		  cy (get center :y))
		(progn
		(line cont (point (+ cx   0) (+ cy   0)) (point (+ cx 100) (+ cy   0)))
		(line cont (point (+ cx 100) (+ cy   0)) (point (+ cx 100) (+ cy 100)))
		(line cont (point (+ cx 100) (+ cy 100)) (point (+ cx   0) (+ cy 100)))
		(line cont (point (+ cx   0) (+ cy 100)) (point (+ cx   0) (+ cy   0)))
	    ))
	)

(def s (surface "test.svg" 128 128))
(surface-status s)
(def c (context s))

;(line c (point 0 0) (point 100 100))
(reg-shape c (point 10 10) 4 100)

(def cx 20)
(def cy 20)
(line c (point (+ cx   0) (+ cy   0)) (point (+ cx 100) (+ cy   0)))
(line c (point (+ cx 100) (+ cy   0)) (point (+ cx 100) (+ cy 100)))
(line c (point (+ cx 100) (+ cy 100)) (point (+ cx   0) (+ cy 100)))
(line c (point (+ cx   0) (+ cy 100)) (point (+ cx   0) (+ cy   0)))

;(line c (point 20 20) (point 120 20))
;(line c (point 120 20) (point 120 120))
;(line c (point 120 120) (point 20 120))
;(line c (point 20 120) (point 20 20))


(surface-clean s c)
