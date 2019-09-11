;(cons 3 (cons 4 (cons 5 null)))
;(list 1 2 4 5)
;(- (+ 1 2 3 (* 4 (/ 8 2))) 1)

;((lambda (x y) (+ x y 10)) 1 3)

;(let (x 3) (+ 1 x))
;(+ 2 3)
;"ciao"
;(str "a b" "c")
;(def cacca 2)
;str
;(progn (+ 1 2) (* 2 4))

;(def test 2)
;(* test 8) 
;(def defn (lamba (name params) )


;(defn square (x) (* x 2))
;(def square (lambda (x) (* x 2)))
;(square 3)

(def surface (svg-surface "test.svg" 128 128))
(svg-status surface)
(def context (svg-context surface))
(svg-line context (list 0 0) (list 100 100))
(svg-clean surface context)


