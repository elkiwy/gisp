(defn point (x y)
	(hashmap :x x :y y))



(def s (surface "test.svg" 128 128))
(surface-status s)
(def c (context s))
(line c (point 0 0) (point 100 100))
(surface-clean s c)


