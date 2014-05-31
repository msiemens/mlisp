; Define a function
(def {func} (lambda {args body} {
    def (head args) (lambda (tail args) body)
}))

(def {func} (lambda {args body} {def (head args) (lambda (tail args) body)}))

; Evaluate a function with arguments passed from a list
; Like javascript's fn.apply
(func {unpack f xs} {
    eval (join (list f) xs)
})

; Evaluate a function with arguments passed as a list
(func {pack f ... xs} {
    f xs
})

; Evaluate the first element from a list
(func {1st list} {
    eval (head list)
})

(func {2nd args} {
    eval (head (tail args))
})

(func {3rd args} {
    eval (head (tail (tail args)))
})

(func {reverse f a b} {
    f b a
})

(func {compose f g ... args} {
    g (unpack f args)
})
