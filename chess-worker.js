let exports;

async function init() {
    const resp = await fetch("./chess.wasm?rand=" + crypto.randomUUID());
    if (!resp.ok) throw new Error(`fetch wasm failed ${resp.status} ${resp.statusText}`);

    const { instance } = await WebAssembly.instantiateStreaming(resp, {});
    exports = instance.exports;
}

await init();
self.postMessage({ type: "ready" });

self.onmessage = (e) => {
    const { id, method, args = [] } = e.data;
    try {
        let value;
        value = exports[method](...args);
        self.postMessage({ id, ok: true, value });
    } catch (err) {
        self.postMessage({
            id, ok: false, error: String(err?.message ?? err)
        });
    }
};
