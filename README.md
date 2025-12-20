

## Build and run in browser

```
make BUILD=release wasm
python3 -m http.server 8000
# open https://localhost:8000/chess.html
```

## Build and run in console

```
make BUILD=release tests
./tests
```

## Missing features and bugs

WASM/web build has an unresolved bug where checkmate and stalemate isn't
rendered or announced
