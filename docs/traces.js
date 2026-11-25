const traces = {
  "drop": [
    {
      "ms": 0,
      "event": "Opening snapshot",
      "exchange_bid": 49,
      "naive_bid": 49,
      "guarded_bid": 49,
      "applied_sequence": 0
    },
    {
      "ms": 100,
      "event": "49-cent deletion lost",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": 49,
      "applied_sequence": 0
    },
    {
      "ms": 140,
      "event": "Gap detected; snapshot requested",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": null,
      "applied_sequence": 0
    },
    {
      "ms": 190,
      "event": "Sequence 3 buffered",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": null,
      "applied_sequence": 0
    },
    {
      "ms": 220,
      "event": "Snapshot 2 installed; sequence 3 replayed",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": 48,
      "applied_sequence": 3
    },
    {
      "ms": 300,
      "event": "Sequence 4 delivered",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": 48,
      "applied_sequence": 4
    },
    {
      "ms": 400,
      "event": "Observation ends",
      "exchange_bid": 48,
      "naive_bid": 49,
      "guarded_bid": 48,
      "applied_sequence": 4
    }
  ],
  "clean": [
    {
      "ms": 0,
      "event": "Opening snapshot",
      "exchange_bid": 49,
      "naive_bid": 49,
      "guarded_bid": 49,
      "applied_sequence": 0
    },
    {
      "ms": 100,
      "event": "49-cent deletion delivered",
      "exchange_bid": 48,
      "naive_bid": 48,
      "guarded_bid": 48,
      "applied_sequence": 1
    },
    {
      "ms": 140,
      "event": "Ask size updated",
      "exchange_bid": 48,
      "naive_bid": 48,
      "guarded_bid": 48,
      "applied_sequence": 2
    },
    {
      "ms": 190,
      "event": "47-cent bid added",
      "exchange_bid": 48,
      "naive_bid": 48,
      "guarded_bid": 48,
      "applied_sequence": 3
    },
    {
      "ms": 300,
      "event": "Sequence 4 delivered",
      "exchange_bid": 48,
      "naive_bid": 48,
      "guarded_bid": 48,
      "applied_sequence": 4
    },
    {
      "ms": 400,
      "event": "Observation ends",
      "exchange_bid": 48,
      "naive_bid": 48,
      "guarded_bid": 48,
      "applied_sequence": 4
    }
  ]
};
