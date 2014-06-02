; Atoms
(def {nil} {})

(def {true}  1)

(def {false} 0)

; Define a function
(def {func} (lambda {args body} {
    def (head args) (lambda (tail args) body)
}))
(def {func} (lambda {args body} {def (head args) (lambda (tail args) body)} ))

; Evaluate a function with arguments passed from a list
; Like javascript's fn.apply
(func {unpack f xs} {
    eval (join (list f) xs)
})

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Functions

; Evaluate a function with arguments passed as a list
(func {pack f ... xs} {
    f xs
})

; Execute a method with the two operands flipped
(func {flip f a b} {
    f b a
})

; Create a function composition -> f(g(...))
(func {compose f g ... args} {
    g (unpack f args)
})

; Run a series of commands and return the last value
(func {do ... l} {
    if (== l nil)
        {nil}
        {last l}
})
(func {do ... l} { if (== l nil) {nil} {last l} })

; Open a new scope
(func {let b} {
    ((lambda {_} b) ())
})
(func {let b} { ((lambda {_} b) ()) })


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Lists

; List length
(func {len l} {
    if (== l nil)
        {0}
        {+ 1 (len (tail l))}
})
(func {len l} { if (== l nil) {0} {+ 1 (len (tail l))} })

; Reverse a list
(func {reverse l} {
    if (== l nil)
        {nil}
        {join (reverse (tail l)) (head l)}
})

; Evaluate the first element from a list
(func {1st l} { eval (head l) })

; Evaluate the second element from a list
(func {2nd l} { eval (head (tail l)) })

; Evaluate the third element from a list
(func {3rd l} { eval (head (tail (tail l))) })

; Evaluate the nth element from a list
(func {nth n l} {
    if (== n 0)
        {1st l}
        {nth (- i 1) (tail l)}
})
(func {nth n l} { if (== n 0) {1st l} {nth (- n 1) (tail l)} })

; Evaluate the last element from a list
(func {last l} { nth (- (len l) 1) l })

; Take N items
(func {take n l} {
    if (== n 0)
        {nil}
        {join (head l) (take (- n 1) (tail l))}
})

; Drop N items
(func {drop n, l} {
    if (== n 0)
        {l}
        {drop (- n 1) (tail l)}
})

; Split at N
(func {split n l} {
    list (take n l) (drop n l)
})

; Is element of list
(func {elem? x l} {
    if (== l nil)
        {false}
        {if (== x (1st l))
            {true}
            {elem? x (tail l)} }
})

; Apply function to list
(func {map f l} {
    if (== l nil)
        {nil}
        {join (list (f (1st l))) (map f (tail f))}
})

; Apply filter to list
(func {filter f l} {
    if (== l nil)
        {nil}
        {join
            (if (f (1st l)) {head l} {nil})
            (filter f (tail l)) }
})

; Fold left
(func {foldl f z l} {
    if (== l nil)
        {z}
        {foldl f (f z (1st l)) (tail l)}
})

; Sum a list of numbers
(func {sum l} {foldl + 0 l})

; Multiply a list of numbers
(func {product l} {foldl * 1 l})

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Conditional functions

(func {bool val} {
    if val
        {true}
        {false}
})

;(func {not x}   {- 1 (bool x)})

;(func {or x y}  {+ (bool x) (bool y)})

;(func {and x y} {* (bool x) (bool y)})

(func {select ... cases} {
    if (== cases nil)
        {error "No cases given!"}
        {if (1st (1st cases))
            {2nd (1st cases)}
            {unpack select (tail cases)} }
})

(func {case var ... cases} {
    if (== cases nil)
        {error "No cases given!"}
        {if (== var (1st (1st cases)))
            {2nd (1st cases)}
            {unpack case (join (list var) (tail cases))} }
})

(def {otherwise} true)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;