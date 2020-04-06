# Gisp
Gisp is a Graphic oriented lisp-like language interpreter written in C.

Gisp inherits most of the functions and syntax rules from Clojure, but
is built for a much more specific and smaller scope than Clojure.

The main goal is to have a fast interpreter capable of parsing gisp
file and generate SVG images through the Cairo library.

Gisp will provide an extensive graphic library full of useful
functions to create, manipulate, and draw basic points, lines, and shapes.


## Language Features
These functions are currently available inside the project:
- **Math**: *add*, *sub*, *mul*, *div*, *bitAnd*, *sin*, *cos*, *dsin*, *dcos*, *deg*, *rad*, *atan2*, *int*, *floor*, *ceil*, *pow*, *modulo*, *sqrt*, *log*, *log10*, *rand*, *seed*, *range*
- **Sequences**: *cons*, *list*, *car*, *cdr*, *count*, *first*, *last*, *get*, *reverse*, *concat*, *assoc*, *vec*, *hashmap*
- **Strings**: *str*
- **I/O**: *read*. *write*
- **Functions and defs**: *lambda*, *defn*, *def*
- **Cairo SVG**: *make-surface*, *make-context*, *surface-status*, *surface-clean*, *line*

These features are also present as macros:
- **quote**
- **if**
- **lambda**
- **apply**
- **def**
- **defn**
- **progn**
- **let**
- **reduce**
- **map**
- **mapv**
- **profile**


## Full Documentation
I'm working to get an automatic documentation system with doc strings and it will be available as soon as possible.


## Todo
- [x] Garbage collection
- [x] Handle String as Gisp Objects
- [ ] Error messages
- [ ] REPL

### Functions
- [ ] partition 
- [ ] conj 
- [ ] filter 
- [ ] empty? 
- [ ] not

## Credits
Gisp is *heavily influenced* by:
 - [Lisp in less than 200 lines of C](https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html) for its core system.
 - [Clojure](https://clojure.org) for the language syntax and core functions.
 - [Generative-toolbelt](https://github.com/elkiwy/generative-toolbelt) for the set of function for primitive shapes.
   



