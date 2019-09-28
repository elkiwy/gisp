

(def PI 3.14159265)

;Creates a Point with absolutes coordinates.
;Returns a Point.
(defn point (x y)
	{ :x x :y y })

;Creates a Vector structure with length `len` and direction `dir` in radians.
(defn vec (len dir)
	{ :len len :dir dir})

;Calculate the distance between two Points.
;Returns a number.
(defn point-distance (a b)
	(let (a2 (pow (- (:x b) (:x a)) 2)
		  b2 (pow (- (:y b) (:y a)) 2))
		(sqrt (+ a2 b2))))

;Calculate the angle between two Points.
;Returns a number.
(defn point-angle (a b)
    (atan2 (- (- (:y b) (:y a))) (- (:x b) (:x a))))
	
;Creates a new intermediate point between two Points `pos` is a floating number between 0 and 1 that defines
;the distance between the two points. 0 returns point `a`,1 returns point `b`, 0.5 returns the middle point between `a` and `b`, etc...
;Returns a Point.
(defn point-between (a b pos)
    (point 
        (+ (:x a) (* pos (- (:x b) (:x a))))
        (+ (:y a) (* pos (- (:y b) (:y a))))))

;Creates a new Point which is moved by a certain ammount in a certain direction defined by a Vector structure.
;Returns a Point.
(defn point-move-by-vector (p vect)
    (point 
        (+ (:x p) (* (:len vect) (cos (:dir vect))))
        (+ (:y p) (* -1 (:len vect) (sin (:dir vect))))))











(defn reg-shape (context center side-n size angle)
	(let (cx   (:x center)
		  cy   (:y center)
		  step (/ (* PI 2) side-n)
		  angles (map #(+ (* % step) (rad angle)) (range side-n))
		  points (map #(point-move-by-vector center (vec size %)) angles)
			 )
		(doseq (i (range (- side-n 1)))
			(line context (get points i) (get points (+ i 1))))
		(line context (last points) (first points))
	    )
	)
