(def PI 3.14159265)

(defn point (x y)
	{ :x x :y y })

(defn vec (len dir)
	{ :len len :dir dir})

(defn point-distance (a b)
	(let (a2 (pow (- (:x b) (:x a)) 2)
		  b2 (pow (- (:y b) (:y a)) 2))
		(sqrt (+ a2 b2))))

(defn point-angle (a b)
    (atan2 (- (- (:y b) (:y a))) (- (:x b) (:x a))))
	
	
(defn point-between (a b pos)
    (point 
        (+ (:x a) (* pos (- (:x b) (:x a))))
        (+ (:y a) (* pos (- (:y b) (:y a))))))

	
	
	


(defn reg-shape (context center side-n size)
	(let (cx   (:x center)
		  cy   (:y center))
		(line context (point (+ cx   0) (+ cy   0)) (point (+ cx 100) (+ cy   0)))
		(line context (point (+ cx 100) (+ cy   0)) (point (+ cx 100) (+ cy 100)))
		(line context (point (+ cx 100) (+ cy 100)) (point (+ cx   0) (+ cy 100)))
		(line context (point (+ cx   0) (+ cy 100)) (point (+ cx   0) (+ cy   0)))
	    )
	)
