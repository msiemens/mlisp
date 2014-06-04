;(load "../basic.sls")
(load "../stdlib/basic.sls")

(testsuite
    ; Functions
    (test {flip (lambda {a b} {- a b}) 1 2} 1)
    (test {unpack (lambda {a b} {+ a b}) {1 2} } 3)
    (test {pack (lambda {a} {head a}) 1 2} {1})
    (test {compose (lambda {x} {+ x 2}) (lambda {x} {* x 2}) 2 } 6)
    (test {do (= {x} 2) x} 2)
    (test {do (= {x} 3) (let {(= {x} 2)}) x} 3)

    ; Lists
    (test {len {1 2 3 4 5}} 5)
    (test {reverse {1 2 3 4 5}} {5 4 3 2 1})
    (test {1st {1 2 3}} 1)
    (test {2nd {1 2 3}} 2)
    (test {3rd {1 2 3}} 3)
    (test {nth 3 {1 2 3 4}} 4)
    (test {last {1 2 3 4}} 4)

    (test {take 2 {1 2 3 4}} {1 2})
    (test {drop 2 {1 2 3 4}} {3 4})
    (test {split 2 {1 2 3 4}} {{1 2} {3 4}})
    (test {elem? 2 {1 2 3 4}} true)
    (test {elem? 7 {1 2 3 4}} false)

    (test {map (lambda {x} {* x 2}) {1 2 3}} {2 4 6})
    (test {filter (lambda {x} {== (% x 2) 0}) {1 2 3}} {2})
    (test {foldl - 0 {1 2 3}} (- 6))

    (test {sum {1 2 3}} 6)
    (test {product {1 2 3}} 6)

    ; Conditionals
    (test {bool 7} 1)
    (test {bool (len {})} 0)
    ; Missing: Tests for select and case
)