
# Notes

## Triangle primitives

There are 3 kinds of triangle primitives, based again on different
interpretations of the vertex stream:

- `GL_TRIANGLES`:

  ```txt
  Indices:     0 1 2 3 4 5 ...
  Triangles:  {0 1 2}
                    {3 4 5}
  ```

- `GL_TRIANGLE_STRIP`:

  ```txt
  Indices:     0 1 2 3 4 5 ...
  Triangles:  {0 1 2}
                {1 2 3} Drawing order is (2 1 3) to maintain proper winding
                  {2 3 4}
                    {3 4 5} Drawing order is (4 3 5) to maintain proper winding
  ```

- `GL_TRIANGLE_FAN`:

  ```txt
  Indices:     0 1 2 3 4 5 ...
  Triangles:  {0 1 2}
              {0} {2 3}
              {0}   {3 4}
              {0}     {4 5}
  ```

### Example

With `gl_Position = proj * view * vec4(gl_VertexID, mod(gl_VertexID, 2), 0.5, 1.0)`.

- `glDrawArrays(GL_TRIANGLES, 0, 6)`:

  ```txt
    1  3 _ 5
   /_\  \ /
  0   2  4
  ```

- `glDrawArrays(GL_TRIANGLE_STRIP, 0, 4)`:

  ```txt
    1 _ 3
   /_\ /
  0   2
  ```

### Source

<https://www.khronos.org/opengl/wiki/primitive>
