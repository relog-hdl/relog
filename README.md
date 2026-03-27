# relog (WIP)

**ACHTUNG!** This project is *very much* work in-progress, and very little
has been done so far. Use at your own risk, if you even manage to do that.

This is a work-in-progress transpiler for Relog, which is an attempt to create
a new HDL which would improve upon Verilog by condensing the syntax and making
code more readable and understandable at a glance, while staying true to the
core ideas that made Verilog good in the first place.

## Project Structure and Subsystems

Project structure is subject to change at this time. The current plan,
however, is as follows.

```
:                                                                             :
:                 |-------|-------|-------------|-------|                     :
:   Microphases   |   1   |   2   |     ...     |   N   |                     :
:                 |-------|-------|-------------|-------|                     :
:                       '                         '                           :
:                            '               '                                :
:                   |----------\------------\-------------\--------------|    :
:        Phases     |    Lex    \   Parse    \   Reform    \    Tidy     |    :
:                   |           /            /             /             |    :
:                   |----------/------------/-------------/--------------|    :
:                   ^           ^            ^             ^             ^    :
:                   |           |            |             |             |    :
:               Relog      Tokens          AST             Modifications |    :
:         source code                on top of             to AST        |    :
:                                       Tokens             on top of     |    :
:                                                          Tokens        /    :
:                   |----------------------|                            /     :
:    Facilities     | Diagnostic Messaging |                           /      :
:                   |----------------------|       Verilog     _______/       :
 '                                                 source code               '
      '                                                                '  
            '                                                    '  
                  '                                        '  
                        '                            '  
     |---------------------\-----------------------|  
     |    Bootstrap App     \     Pipeline App     |            OS-like  
     |  (sets up services)  /  (utilizes services) |        "boot" flow  
     |---------------------/-----------------------|  

-------------------------------------------------------------------------------

               |---------|  |--------|  |-------------|  |------------------|  
    Services   | Logging |  | Config |  | Source Code |  | Compilation Code |  
	           |         |  |        |  |    Tree     |  |       Tree       |  
               |---------|  |--------|  |-------------|  |------------------|  

               |--------|  |----------|  
	     Lib   | Domain |  | File I/O |  
               |--------|  |----------|  


		Util            (only things which should have been std::x)  

```

The project will be built as two applications, the bootstrap app and
the pipeline app. The bootstrap will set up the services which the pipeline
app will use.

The app layer will be built on top of the service layer, and the service layer
will be built on top of the library layer, which may use components from the
util layer.

The philosophy of the layered architecture is pretty standard: any component
of a layer may use anything from any layers beneath it, but not from any other
components of the same layer or from any layer above it.

The pipeline application will be conceptually split into the facilties layer
and the phases layer, with phases being the meat of the whole project.

There is currently only one facility planned for implementation, the diagnostic
messaging facility, which will provide diagnostic message logging capabilities
to all phases and will be built on top of the logging layer, with additional
structures and helper functions for diagnostic messages, specifically.

The purpose of each phase should, I think, be obvious from the name to pretty
much anyone familiar with basics of compiler design, but in case it is not:

* Lex Phase: interprets the character stream, producing tokens, which are
  linked to character sequences and have a specific lexical meaning; checks
  for syntax errors
* Parse Phase: interprets the token stream, building an abstract syntax tree
  and attempting to make sense of the code on a semantic level; checks for
  semantic errors
* Reform Phase: modifies the abstract syntax tree using pre-defined patterns
  and procedures to produce only verilog-compatible constructs
* Tidy Phase: performs final clean up, decides on the formatting for the
  verilog code and writes its results to output character stream

Each phase may or may not be split into microphases, the amount and functions
of which are undetermined at this point.

## Subsystem Status

| Lib      | Location         | Status               |
|----------|------------------|----------------------|
| Domain   | `src/lib/domain` | usable, needs polish |
| File I/O | `src/lib/io`     | nonexistant          |

| Service               | Location               | Status                  |
|-----------------------|------------------------|-------------------------|
| Logging               | `src/service/logging`  | skeleton                |
| Config                | `src/service/config`   | nonexistant             |
| Source File Tree      | `src/service/codetree` | skeleton, needs rewrite |
| Compilation File Tree | `src/service/codetree` | skeleton, needs rewrite |

| Application | Location        | Status\*    |
|-------------|-----------------|-------------|
| Bootstrap   | `src/bootstrap` | nonexistant |
| Pipeline    | `src/pipeline`  | nonexistant |

\*status of applications indicates the state of the control flow code, and not
of individual components which comprise the application and are invoked in the
control flow

| Facility             | Location                        | Status      |
|----------------------|---------------------------------|-------------|
| Diagnostic Messaging | `src/pipeline/facility/diagmsg` | nonexistant |

| Phase  | Location\*                  | Status      |
|--------|-----------------------------|-------------|
| Lex    | `src/pipeline/phase/lex`    | nonexistant |
| Parse  | `src/pipeline/phase/parse`  | nonexistant |
| Reform | `src/pipeline/phase/reform` | nonexistant |
| Tidy   | `src/pipeline/phase/tidy`   | nonexistant |

\*note that the names of the phases have been chosen carefully to make their
order, when sorted alphabetically by name, the same as their execution order
