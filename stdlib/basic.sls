; Atoms
(def {nil} {})

(def {true}  1)

(def {false} 0)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Functions

; Define a function
(def {function} (lambda {args body} {
    def (head args) (lambda (tail args) body)
}))

; Evaluate a function with arguments passed from a list
; Like javascript's fn.apply
(function {unpack f xs} {
    eval (join (list f) xs)
})

; Evaluate a function with arguments passed as a list
(function {pack f ... xs} {
    f xs
})

; Execute a method with the two operands flipped
(function {flip f a b} {
    f b a
})

; Create a function composition -> f(g(...))
(function {compose f g ... args} {
    f (unpack g args)
})

; Run a series of commands and return the last value
(function {do ... l} {
    if (== l nil)
        {nil}
        {last l}
})

; Open a new scope
(function {let b} {
    ((lambda {_} b) ())
})


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Lists

; List length
; TODO: Maybe implement in C for performance?
(function {len l} {
    if (== l nil)
        {0}
        {+ 1 (len (tail l))}
})
(function {len l} { if (== l nil) {0} {+ 1 (len (tail l))} })

; Reverse a list
(function {reverse l} {
    if (== l nil)
        {nil}
        {join (reverse (tail l)) (head l)}
})

; Evaluate the first element from a list
(function {1st l} { eval (head l) })

; Evaluate the second element from a list
(function {2nd l} { eval (head (tail l)) })

; Evaluate the third element from a list
(function {3rd l} { eval (head (tail (tail l))) })

; Evaluate the nth element from a list
(function {nth n l} {
    if (== n 0)
        {1st l}
        {nth (- n 1) (tail l)}
})

; Evaluate the last element from a list
(function {last l} { nth (- (len l) 1) l })

; Take N items
; TODO: Tests
(function {take n l} {
    if (== n 0)
        {nil}
        {join (head l) (take (- n 1) (tail l))}
})

; Drop N items
(function {drop n l} {
    if (== n 0)
        {l}
        {drop (- n 1) (tail l)}
})

; Split at N
(function {split n l} {
    list (take n l) (drop n l)
})

; Is element of list
(function {elem? x l} {
    if (== l nil)
        {false}
        {if (== x (1st l))
            {true}
            {elem? x (tail l)} }
})

; Apply function to list
(function {map f l} {
    if (== l nil)
        {nil}
        {join (list (f (1st l))) (map f (tail l))}
})

; Apply filter to list
(function {filter f l} {
    if (== l nil)
        {nil}
        {join
            (if (f (1st l)) {head l} {nil})
            (filter f (tail l)) }
})

; Fold left
(function {foldl f z l} {
    if (== l nil)
        {z}
        {foldl f (f z (1st l)) (tail l)}
})

; Sum a list of numbers
(function {sum l} {foldl + 0 l})

; Multiply a list of numbers
(function {product l} {foldl * 1 l})

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Conditional functions

(function {bool val} {
    if val
        {true}
        {false}
})

(function {select ... cases} {
    if (== cases nil)
        {error "No cases given!"}
        {if (1st (1st cases))
            {2nd (1st cases)}
            {unpack select (tail cases)} }
})

(function {case var ... cases} {
    if (== cases nil)
        {error "No cases given!"}
        {if (== var (1st (1st cases)))
            {2nd (1st cases)}
            {unpack case (join (list var) (tail cases))} }
})

(def {otherwise} true)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Unit tests

(function {test cmd expected} {
    do  (= {result} (eval cmd))
        (if (!= (eval cmd) expected)
            {do (println   "Test" cmd "==" expected "failed, actual result:" result)
                (false) }
            {true} )
})

(function {testsuite ... tests} {
    do
        (= {test_count}   (len tests))
        (= {failed_count} (len (filter (lambda {result} {== result 0}) tests)))
        (if failed_count {println ""})
        (println (- test_count failed_count) "tests passed")
        (select
            {(== failed_count 0)       ()}
            {(== failed_count 1)       (println "1 test failed")}
            {true (println failed_count "tests failed")} )
        (failed_count)
})