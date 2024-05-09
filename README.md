# CS 644 Compiler Construction
Course website: https://student.cs.uwaterloo.ca/~cs444/


## Project Overview
The overall project for the course is to write a compiler from Joos 1W a fairly large subset of Java, to i386 assembly language (the Netwide Assembler dialect). The project will be done in groups of three or four. The project will be broken up into several assignments with fixed due dates. The assignments must be submitted to the Marmoset on-line submission and testing system.

## Assignment 1: Scanning, Parsing, Weeding
In this assignment, you will implement lexical and syntactic analysis. The scanner splits up the input into tokens and catches lexical errors (inputs that cannot form valid tokens). The parser checks that the input list of tokens conforms to a syntax specified using a context-free grammar. The weeding stage detects simple errors that generally could have been checked by a context-free grammar, but this would make the grammar very complicated. It is recommended that your design follow the above three stages, though it is not required, as long as you somehow implement the lexical and syntactic specification of Joos 1W.

The following restrictions of the Joos 1W language must be checked (either in the parser or in the weeder):

- All characters in the input program must be in the range of 7-bit ASCII (0 to 127).
- A class cannot be both abstract and final.
- A method has a body if and only if it is neither abstract nor native.
- An abstract method cannot be static or final.
- A static method cannot be final.
- A native method must be static.
- The type void may only be used as the return type of a method.
- A class/interface must be declared in a .java file with the same base name as the class/interface.
- An interface cannot contain fields or constructors.
- An interface method cannot be static, final, or native.
- An interface method cannot have a body.
- Every class must contain at least one explicit constructor.
- No field can be final.
- No multidimensional array types or multidimensional array creation expressions are allowed.
- A method or constructor must not contain explicit this() or super() calls.

This phase of the compiler should also determine the values of all literals in the program. For integer literals, it must check that the literal is within the range of a Java int. For character and string literals, it must expand any escape sequences in the literal.

Your parser must be implemented using an LALR(1) parser generator, such as CUP, Bison, and JLALR. Debugging conflicts reported by a parser generator can be challenging, especially when the parser generator reports only conflict items and the lookahead symbol. It is strongly recommended that you use a parser generator that provides counterexamples which better explain parsing conflicts. Bison and an extended version of CUP currently support counterexample generation.

## Assignment 2: Static Checking (Part A)
In this assignment, you will implement a compiler phase that checks whether a Joos 1W program satisfies certain basic validity requirements. It is recommended that your design follow these four stages: abstract syntax tree building, environment building, type name resolution, and hierarchy checking.

### Abstract Syntax Tree Building
A parse tree built using an unambiguous grammar typically contains many redundant nodes. By recursively traversing the parse tree, it is possible to build an abstract syntax tree with an analogous structure but fewer nodes. An abstract syntax tree simplifies later phases of the compiler by reducing the number of nodes and the number of different kinds of nodes that each phase needs to handle. When a compiler is implemented using a statically typed language, distinct types are typically defined for each possible kind of tree nodes, so that later compiler phases can use the type to document the tree nodes that they handle.

### Environment Building
The environment building stage creates environments (containing classes, interfaces, fields, methods, local variables, and formal parameters) for each scope. Given a name of one of these entities, the environment should be able to locate the correct declaration of the entity.

After constructing all of the environments, the following requirements of the Joos 1W language must be checked:

- No two fields declared in the same class may have the same name.
- No two local variables with overlapping scope have the same name.
- No two classes or interfaces have the same canonical name.
### Type Linking
The type linking stage connects each use of a named type (class or interface) to the declaration of the type. At this stage, only names that can be syntactically (according to JLS 6.5.1) determined to be names of types need to be linked. Some names are syntactically ambiguous, in the sense that type checking must be done before it can be determined whether they are names of types or of other entities (see JLS 6.5). These ambiguous names will be linked in a later assignment.

When linking type names, the following requirements of the Joos 1W language must be checked:

- No single-type-import declaration clashes with the class or interface - declared in the same file.
- No two single-type-import declarations clash with each other.
- All type names must resolve to some class or interface declared in some - file listed on the Joos command line.
- All simple type names must resolve to a unique class or interface.
- When a fully qualified name resolves to a type, no strict prefix of the - fully qualified name can resolve to a type in the same environment.
- No package names—including their prefixes—of declared packages, - single-type-import declarations or import-on-demand declarations that - are used may resolve to types, except for types in the default, unnamed - package.
- Every import-on-demand declaration must refer to a package declared in some file listed on the Joos command line. That is, the import-on-demand declaration must refer to a package whose name appears as the package declaration in some source file, or whose name is a prefix of the name appearing in some package declaration.
### Hierarchy Checking
The fourth stage computes the inheritance relationships for classes, interfaces, methods, and fields, and checks that they conform to the various rules given in Chapters 8 and 9 of the Java Language Specification. Specifically, this stage should check that:

- A class must not extend an interface. (JLS 8.1.3)
- A class must not implement a class. (JLS 8.1.4)
- An interface must not be repeated in an implements clause or in an - extends clause of an interface. (JLS 8.1.4)
- A class must not extend a final class. (JLS 8.1.1.2, 8.1.3)
- An interface must not extend a class. (JLS 9.1.2)
- The hierarchy must be acyclic. (JLS 8.1.3, 9.1.2)
- A class or interface must not declare two methods with the same - signature (name and parameter types). (JLS 8.4, 9.4)
- A class must not declare two constructors with the same parameter types - (JLS 8.8.2)
- A class or interface must not contain (declare or inherit) two methods - with the same signature but different return types (JLS 8.1.1.1, 8.4, 8.- 4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
- A class that contains (declares or inherits) any abstract methods must - be abstract. (JLS 8.1.1.1)
- A nonstatic method must not replace a static method. (JLS 8.4.6.1)
- A static method must not replace a nonstatic method. (JLS 8.4.6.2)
- A method must not replace a method with a different return type. (JLS 8.- 1.1.1, 8.4, 8.4.2, 8.4.6.3, 8.4.6.4, 9.2, 9.4.1)
- A protected method must not replace a public method. (JLS 8.4.6.3)
- A method must not replace a final method. (JLS 8.4.3.3)

## Assignment 3: Static Checking (Part B)
In this assignment, you will implement disambiguation of names and type checking.

### Disambiguation of Names
The disambiguation stage determines the meaning of each remaining use of a name and links the use to the declaration of the entity (field, method, local variable, formal parameter, or type) that the name refers to. Disambiguation of syntactically ambiguous names is specified in Section 6.5.2 of the Java Language Specification and was discussed in class.

Note that Joos 1W supports a simplified version of method overloading compared to the full Java overloading rules detailed in Section 15.12.2 of the Java Language Specification. Although Joos 1W does allow method overloading (i.e., multiple methods with the same name, but different parameter types), the declared types of the parameters must exactly match the static types of the argument expressions in order for the method to be called. For example, a call such as bar('x') will not invoke a method with signature bar(int), as it would in Java, but the call bar((int)'x') would.

If a prefix of a name refers to a variable or field, then the remaining part of the name refers to non-static field(s) or a method of the object referred to by that variable or field. At this stage, non-static fields and methods are not resolved; it is deferred until the type checking stage, when the static type of the base variable or field is known.

During and/or after disambiguation of names, the following restrictions of the Joos 1W language must be checked:

- Check that all names, except for non-static field and method accesses, can be disambiguated. It is an error if a name cannot be resolved to any entity that is in scope at the point where the name is used.
- Check the rules specified in Section 8.3.2.3 of the Java Language Specification regarding forward references. The initializer of a non-static field must not use (i.e., read) by simple name (i.e., without an explicit this) itself or a non-static field declared later in the same class.

### Type Checking
The type checking stage determines the types of all expressions and subexpressions, checks that they conform to the Joos 1W typing rules, and links all remaining names (i.e., non-static field accesses and method calls) to the declarations that they refer to.

It is possible for a class to contain multiple methods with the same signature, all abstract, and all inherited from one of its superclasses. In this case, the method call may be linked to any one of these methods, since they are guaranteed to have the same return type.

During and/or after type checking, the following restrictions of the Joos 1W language must be checked:

- Check that the types of all expressions and subexpressions conform to the Joos 1W typing rules and that all statements are type-correct. The Joos 1W typing rules are the same as the Java typing rules with the following exceptions:
    - Assignability is the same as in Java (see Section 5.2 of the Java Language Specification), except that narrowing conversions are never allowed.
    - The rules for casts to reference types are simplified relative to the rules specified in Section 5.5 of the Java Language Specification. In a cast to a reference type T, the type of the subexpression S must be such that either S is assignable to T, T is assignable to S, or one of S or T is an interface and the other is either an interface or a non-final class. The rules for casts to primitive types are the same as in Java. When S and T are reference types, a subexpression of type S[] may be cast to type T[] if and only if a subexpression of type S may be cast to type T. Like in Java, the Joos 1W typing rules for instanceof expressions and equality comparisons (== and !=) are defined in terms of the rules for casts.
- Check that all non-static field and method uses can be resolved to fields and methods that exist.
- Check that fields/methods accessed as static are actually static, and that fields/methods accessed as non-static are actually non-static.
- Check that all accesses of protected fields, methods and constructors are in a subtype of the type declaring the entity being accessed, or in the same package as that type.
- Check that the name of a constructor is the same as the name of its enclosing class.
- A constructor in a class other than java.lang.Object implicitly calls the zero-argument constructor of its superclass. Check that this zero-argument constructor exists.
- Check that no objects of abstract classes are created.
- Check that no bitwise operations occur.
- Check that the implicit this variable is not accessed in a static method or in the initializer of a static field.

### Assignment 4: Static Checking (Part C)
In this assignment, you will implement the static checking of the following requirements of the Joos 1W language:

- All statements must be reachable. (Details of the exact definition of reachability are specified in Section 14.20 of the Java Language Specification. It turns out that it is possible to implement reachability checking using an AST traversal, but it is recommended that you implement it as a dataflow analysis.)
- The last statement on every finite-length execution path through a method whose return type is not void must be a return statement.
- Every local variable must have an initializer, and the variable must not occur in its own initializer.
- You will also implement a dataflow analysis to emit warnings for dead assignments to local variables. Notice that since Joos 1W requires that all local variables be initialized when declared, your compiler need not emit warnings for any local variable declaration even though the initialization may be dead. Any warnings emitted by your compiler should be for assignments to already initialized local variables.

### Assignments 5: Code Generation (Part A)
In this assignment, you will make your compiler generate i386 assembly (Intel dialect) for the subset of Joos 1W language that does not include object-oriented features. This subset does include arrays, static methods, and static fields.

Your compiler should first generate IR code, then lower it to canonical form, and finally emit assembly code by tiling the IR syntax trees. You are expected to make use of x86 features, including memory operands and rich addressing modes, in designing your tiles. You need not implement register allocation in this assignment; it is acceptable to spill all variables to the stack.

A Java definition of the IR used in lectures can be found under /u/cs444/pub/tir in the linux.student.cs environment. AST types and an interpreter are included. You can change its definition or reimplement it in your chosen implementation language, but you should document your changes in your report. Your IR should not deviate too much from the provided intermediate representation; for example, using LLVM or JVM is not allowed.

### Assignment 6: Code Generation (Part B)
In this assignment, you will build on your Assignment 5 compiler and implement code generation (i386 assembly, Intel dialect) for the complete Joos 1W language, including its object-oriented features.

You will also implement register allocation, either using linear scan or graph coloring. By default, your compiler should perform register allocation. Your joosc compiler should also provide a command-line option, --opt-none, that disables optimizations including register allocation. You will evaluate the performance improvements enabled by register allocation on your own benchmarks.

You are encouraged to implement optimizations at the IR level, such as constant propagation and folding, method inlining, and common subexpression elimination. Groups whose compilers are the most correct and generate the fastest code may be rewarded.


