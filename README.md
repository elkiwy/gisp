# Gisp
Gisp is a Graphic oriented lisp-like language interpreter written in C.

Gisp inherits most of the functions and syntax rules from Clojure, but
is built for a much more specific and smaller scope than Clojure.

The main goal is to have a fast interpreter capable of parsing gisp
file and generate SVG images through the Cairo library.

Gisp provides an extensive graphic library full of useful
functions to create, manipulate, and draw basic points, lines, and shapes.

## Full Documentation
All the language documentation is currenly hosted on my website here: [Gisp Documentation](https://elkiwyart.com/site/gisp_documentation.html).

## Installation
Once cloned the repository you should be able to install Gisp by compiling it and installing into your system with these commands:
```
cd gisp
make
make install
```
This will place an executable named "gisp" into your `/usr/local/bin`.
Once installed you can use it by executing:
```
gisp path/to/your/file.gisp
```

## Hello Square
To give you a brief glimpse at how Gisp works, here you have a simple
example on how to generate an .svg file with a simple square.

```clojure
;Creates the file and its canvas of 1024x1024.
(def s (make-surface "example.svg" 1024 1024))

;Create the context of the surface, this will be used to draw.
(def c (make-context s))

;Define out simple function to draw a rectangle.
(defn draw-rect (x1 y1 x2 y2)
	(draw-line c (point x1 y1) (point x2 y1))
	(draw-line c (point x2 y1) (point x2 y2))
	(draw-line c (point x2 y2) (point x1 y2))
	(draw-line c (point x1 y2) (point x1 y1)))

;Execute out function.
(draw-rect 100 100 300 300)

;Create also a png version of the image, this is optional but very convenient.
(surface-to-png s "example.png")

;Clean the surface finishing all the work.
(surface-clean s c)
```


## Gisp Editor
Other than Gisp core, I'm also developing a custom editor to easily
interact with Gisp. This is still needs some work to be usable from
everyone and right now I'm focussing more on the interpreter.

![GispEditor](/images/GispEditor.png)

## Missing feature to be added in the future
 - Verbose error message to help debugging
 - A REPL to evaluate code interactivly
 - General improvement to make it better for new users

## Credits
Gisp is *heavily influenced* by:
 - [Lisp in less than 200 lines of C](https://carld.github.io/2017/06/20/lisp-in-less-than-200-lines-of-c.html) for its core system.
 - [Clojure](https://clojure.org) for the language syntax and core functions.
 - [Generative-toolbelt](https://github.com/elkiwy/generative-toolbelt) for the set of function for primitive shapes.
   
## Images and links
 - Every recent images posted on [Instagram](https://instagram.com/elkiwy) are made with Gisp.
 - You can find other information about Gisp here [ElkiwyArt.com](https://elkiwyart.com/site/gisp.html) are made with Gisp.

