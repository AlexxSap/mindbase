1. **%v**: The default format, depending on the argument type.

2. **%+v**: Same as `%v` but will include the field names in the default formatter for structs.

3. **%#v**: A representation of the Go-syntax for the value.

4. **%T**: Type of the value.

5. **%d**: Integer (base 10).

6. **%b**: Integer (base 2, binary).

7. **%o**: Integer (base 8, octal).
    
8. **%x**, **%X**: Integer (base 16, hex). Uses lowercase or uppercase letters for a-f respectively.

9. **%f**, **%F**: Floating-point number.

10. **%e**, **%E**: Scientific notation.

11. **%g**: Decides `%e` or `%f` based on magnitude.

12. **%s**: String.

13. **%q**: A double-quoted string safely escaped with Go syntax.

14. **%p**: Pointer.

15. **%c**: The character represented by the corresponding Unicode code point.
