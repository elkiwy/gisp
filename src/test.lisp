;(defn point (x y)
;	(hashmap :x x :y y))
;
;(def PI 3.14159265)
;
;(defn reg-shape (cont center side-n size)
;	(let (cx (get center :x)
;		  cy (get center :y))
;		(progn
;		(line cont (point (+ cx   0) (+ cy   0)) (point (+ cx 100) (+ cy   0)))
;		(line cont (point (+ cx 100) (+ cy   0)) (point (+ cx 100) (+ cy 100)))
;		(line cont (point (+ cx 100) (+ cy 100)) (point (+ cx   0) (+ cy 100)))
;		(line cont (point (+ cx   0) (+ cy 100)) (point (+ cx   0) (+ cy   0)))
;	    ))
;	)


(def x "def ")
(write (str "def " x))

(let (x "let ")
	(progn 
		(write (str "let " x))
		(let (x "LET " x (str x "LOT "))
			(write (str "LET " x))
		)
		(write (str "let " x))
	)
)


(write (str "boh " x))



;(def s (surface "test.svg" 128 128))
;(surface-status s)
;(def c (context s))
;(reg-shape c (point 10 10) 4 100)
;(surface-clean s c)
