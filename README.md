# relog (WIP)

**ACHTUNG!** This project is *very much* work in-progress, and very little
has been done so far. Use at your own risk, if you even manage to do that.

This is a work-in-progress transpiler for Relog, which is an attempt to create
a new HDL which would improve upon Verilog by condensing the syntax and making
code more readable and understandable at a glance, while staying true to the
core ideas that made Verilog good in the first place.

## Project Structure and Subsystems

Project structure is subject to change at this time.

```
:   --                                                      --                :
:   |               |-------|-------|-------------|-------|  |                :
:   | Microphases   |   1   |   2   |     ...     |   N   |  |   (Optional)   :
:   |               |-------|-------|-------------|-------|  |                :
:   --                    '                         '       --                :
:                              '               '                              :
:            |----------\--------\------------\-------------\--------------|  :
:    Phases  |  Inliner  \  Lex   \   Parse    \   Reform    \    Tidy     |  :
:            | +Decanter /        /            /             /             |  :
:            |----------/--------/------------/-------------/--------------|  :
:            ^          ^        ^            ^             ^              ^  :
:            |          |        |            |             |              |  :
:        Relog      Cleaned up   Tokens      AST       Modified AST        |  :
:  source code        source                                               /  :
:                                                                         /   :
:                                                      Verilog     ______/    :
:                     |----------------------|         source code            :
:      Facilities     | Diagnostic Messaging |                                :
:                     |----------------------|                                :
 '                                                                           '
      '                                                                '  
            '                                                    '  
                  '                                        '  
                        '                            '  
     |---------------------\-----------------------|  
     |      Bootstrap       \       Pipeline       |     Initialization flow
     |  (sets up services)  /  (utilizes services) |  
     |---------------------/-----------------------|  

-------------------------------------------------------------------------------

               |---------|  |--------|  |------------|  
    Services   | Logging |  | Config |  |   Source   |  
               |         |  |        |  |   Stack    |  
               |---------|  |--------|  |------------|  

               |--------|  |----------|  
         Lib   | Domain |  |  Source  |  
               |--------|  | File I/O |  
                           |----------|

		Util        (only things which should really have been std::x)  

```

The project is built as two applications, the bootstrap application and
the pipeline application. The bootstrap sets up the services which the pipeline
application uses.

The application layer is built on top of the service layer, and the
service layer is built on top of the library layer, which uses components from
the util layer.

The philosophy of the layered architecture is pretty standard: any component
of a layer may use anything from any layers beneath it, but not from any other
components of the same layer or from any layer above it.

The pipeline application is conceptually split into the facilties layer
and the phases layer, with phases being the meat of the whole project.

There is currently only one facility planned for implementation, the diagnostic
messaging facility, which will provide diagnostic message logging capabilities
to all phases and will be built on top of the logging layer, with additional
structures and helper functions for diagnostic messages, specifically.

The purposes of the phases are as follows:

* Inliner Phase: removes comments from the character stream, then interprets
  preprocessor directives and manages the character stream accordingly
* Lex Phase: interprets the character stream, producing tokens, which are
  linked to character sequences and have a specific lexical meaning
* Parse Phase: interprets the token stream, building an abstract syntax tree
  and attempting to make sense of the code on a semantic level
* Reform Phase: modifies the abstract syntax tree using pre-defined patterns
  and procedures to reduce the tree to only verilog-compatible constructs
* Tidy Phase: performs final clean up, decides on the formatting for the
  verilog code and writes its results to output character stream

Each phase may or may not be split into microphases, the amount and functions
of which are undetermined at this point.

## Subsystem Status

| Lib             | Location         | Status               |
|-----------------|------------------|----------------------|
| Domain          | `src/lib/domain` | usable               |
| Source File I/O | `src/lib/io`     | nonexistant          |

| Service               | Location               | Status                  |
|-----------------------|------------------------|-------------------------|
| Logging               | `src/service/logging`  | usable                  |
| Config                | `src/service/config`   | nonexistant             |
| Source Stack          | `src/service/source`   | skeleton, needs rewrite |

| Application | Location        | Status\*    |
|-------------|-----------------|-------------|
| Bootstrap   | `src/bootstrap` | preliminary |
| Pipeline    | `src/pipeline`  | preliminary |

\*status of applications indicates the state of the control flow code, and not
of individual components which comprise the application and are invoked in the
control flow

| Facility             | Location                        | Status      |
|----------------------|---------------------------------|-------------|
| Diagnostic Messaging | `src/pipeline/facility/diagmsg` | nonexistant |

| Phase   | Location\*                   | Status      |
|---------|------------------------------|-------------|
| Inliner | `src/pipeline/phase/inliner` | preliminary |
| Lex     | `src/pipeline/phase/lex`     | preliminary |
| Parse   | `src/pipeline/phase/parse`   | preliminary |
| Reform  | `src/pipeline/phase/reform`  | nonexistant |
| Tidy    | `src/pipeline/phase/tidy`    | nonexistant |

\*note that the names of the phases have been chosen carefully to make their
order, when sorted alphabetically by name, the same as their execution order
