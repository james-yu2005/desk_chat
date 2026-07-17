# Tavily Search MCP for Desk Chat

Connects [Tavily](https://tavily.com/) web search to Desk Chat via the
[xiaozhi.me](https://xiaozhi.me) cloud MCP endpoint.

```
You → Desk Chat → xiaozhi.me → tavily-search MCP → spoken summary
```

## Prerequisites

| Requirement | Notes |
|-------------|-------|
| [xiaozhi.me](https://xiaozhi.me) account | Device already paired |
| MCP endpoint URL | Console → agent → MCP endpoint |
| [Tavily API key](https://app.tavily.com/) | Free tier ~1,000 credits/month |
| Node.js **v20+** | For `npx tavily-mcp` |
| Python **3.10+** | For `mcp_pipe.py` |

## Setup

```bash
cd scripts/mcp_search
cp .env.example .env
# Edit .env — set MCP_ENDPOINT and TAVILY_API_KEY
./start.sh
```

Leave the terminal open. Try:

- “Search for what’s new in ESP-IDF 5.5.”
- “What are the latest news about AI voice assistants?”

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `MCP_ENDPOINT` not set | Create `.env` from `.env.example` |
| `TAVILY_API_KEY` not set | Add key from Tavily dashboard |
| `npx not found` | Install Node.js 20+ from [nodejs.org](https://nodejs.org/) |
| Search not triggered | Confirm `start.sh` is running; ask to “search the web” |

## Files

| File | Purpose |
|------|---------|
| `mcp_pipe.py` | WebSocket ↔ stdio bridge to xiaozhi |
| `mcp_config.json` | Tavily MCP server definition |
| `.env` | Secrets (gitignored) |
| `start.sh` | Launcher |
