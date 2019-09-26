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
