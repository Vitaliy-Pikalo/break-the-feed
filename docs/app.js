const get = id => document.getElementById(id);
let index = 0;

function render() {
    const frames = traces[get("mode").value];
    const f = frames[index];
    const paused = f.guarded_bid === null;
    const naiveWrong = f.naive_bid !== f.exchange_bid;
    const guardWrong = !paused && f.guarded_bid !== f.exchange_bid;
    get("exchange").textContent = f.exchange_bid + "¢";
    get("naive").textContent = f.naive_bid + "¢";
    get("guard").textContent = paused ? "—" : f.guarded_bid + "¢";
    get("naiveState").textContent = naiveWrong ? "Incorrect bid" : "Matches exchange";
    get("guardState").textContent = paused ? "Quote suppressed" : guardWrong ? "Gap still undetected" : "Matches exchange";
    get("naiveState").className = naiveWrong ? "wrong" : "";
    get("guardState").className = paused || guardWrong ? "wrong" : "";
    get("event").textContent = f.event;
    get("time").textContent = f.ms + " ms";
    get("slider").max = frames.length - 1;
    get("slider").value = index;
    get("prev").disabled = index === 0;
    get("next").disabled = index === frames.length - 1;
    let naive = 0, guarded = 0, suppressed = 0;
    for (let i = 0; i < index; ++i) {
        const prev = frames[i], dt = frames[i+1].ms - prev.ms;
        if (prev.naive_bid !== prev.exchange_bid) naive += dt;
        if (prev.guarded_bid === null) suppressed += dt;
        else if (prev.guarded_bid !== prev.exchange_bid) guarded += dt;
    }
    for (const [id, ms] of [["nw", naive], ["gw", guarded], ["off", suppressed]])
        get(id).textContent = ms + " ms";
    get("log").replaceChildren();
    for (const row of frames.slice(0, index + 1)) {
        const tr = get("log").insertRow();
        if (row === f) tr.className = "current";
        for (const val of [row.ms + " ms", row.event, row.applied_sequence])
            tr.insertCell().textContent = val;
    }
}
get("mode").onchange = () => { index = 0; render(); };
get("slider").oninput = e => { index = Number(e.target.value); render(); };
get("prev").onclick = () => { --index; render(); };
get("next").onclick = () => { ++index; render(); };
render();
