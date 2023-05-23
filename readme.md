
# Chess Diagrams

This software is made with ❤️ by
[thechessnerd](https://www.instagram.com/thechessnerd/) team.

Create chess diagrams with the `Alpha Chess` Set and a wooden background
in a $800\times800$ optimized `.png` file with the chosen perspective.
Create $400\times400$ `.gif` themed game previews with move highlights.
The valid themes are `bubble`, `iceage`, `nature`, `wooden`, `grapes`.
The resulting files are optimized for web use.

### Static Preview

The static diagram can be achieved by adding the
[FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
(or part of it) to the endpoint base:

```
http://localhost:8080/fen/white/rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
http://localhost:8080/fen/black/rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
```

### Animated Previews

The animated previews can be achieved by locating the resource at the base
endpoint, followed by the theme name, the perspective and the list of moves.
Moves for each frame are indicated by the starting square and the ending
square. When queening they are followed by an equal sign and the resulting
piece (`e7e8=Q`). When castling specify the king move only, from start to
end square (`e1g1`). Illegal moves result in undefined behaviour. Caslting
highlights are displayed for the king alone, but this setting can be changed
by uncommenting the corresponding sections in the code.
Each frame ends with a semicolon:

```
http://localhost:8080/pgn/nature/white/e2e4;e7e5;g1f3
http://localhost:8080/pgn/nature/black/e2e4;e7e5;g1f3
```

### Rights

- For the `.png` encoding we depend on the **LOVELY** 🤎
[lodepng](https://github.com/lvandeve/lodepng)
- For the `.gif` encoding we depend on the **AMAZING** 💜
[msf_gif](https://github.com/notnullnotvoid/msf_gif)
- For the server routing we depend on the **GREAT** 🛣️
[route66](https://github.com/r-lyeh-archived/route66)

> The rights of the `Alpha Chess` Set belong to **Eric Bentzen** and have been
legally purchased under the name of Zachary Saine. Do not use the chess set
if you do not own the rights.
