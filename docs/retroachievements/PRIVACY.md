# NextUI Privacy Policy — RetroAchievements Integration

**Last updated:** 2026-06-09
**Applies to:** NextUI v6.12.0 and later, on all supported platforms (Trimui Brick, Smart Pro, Smart Pro S).

This policy describes the personal data NextUI handles when its
RetroAchievements (RA) integration is used. NextUI is free, open-source
software released under GPL-3.0; the project operates no servers, runs
no analytics, and ships no advertising or tracking code.

---

## Summary (TL;DR)

- **NextUI itself collects nothing.** No telemetry, no analytics, no
  crash reporting, no usage metrics. Verifiable in the source tree:
  no analytics SDKs, no outbound HTTP destinations except
  `retroachievements.org` (when the user enables RA) and core/asset
  download endpoints invoked manually by the user from the Pak Store.
- **When RA is enabled, you authenticate with retroachievements.org
  using your own RA account.** Data flows directly between your device
  and RA's servers; NextUI is not in the middle.
- **All RA-related data NextUI keeps lives on your SD card.** It is
  removed by deleting the file (`Saves/.userdata/[platform]/minui.cfg`
  and the `Saves/.userdata/[platform]/.ra/` directory) or by reflashing
  the SD card. NextUI has no remote-wipe mechanism because there is
  nothing remote to wipe.

---

## What NextUI stores locally

When you log in to RetroAchievements via the Settings application
(Tools → Settings → RetroAchievements), the following are written to
your SD card:

| Field | Location | Purpose | Lifetime |
|---|---|---|---|
| RA username | `minui.cfg` (`raUsername=`) | Pre-fills the login screen on next launch | Until you log out or clear the field |
| RA password | `minui.cfg` (`raPassword=`) | Re-authentication if the session token expires | Until you log out or clear the field |
| RA session token | `minui.cfg` (`raToken=`) | Avoids re-sending password on each launch | Until logout or RA-server-side expiry |
| RA canonical username | `minui.cfg` (`raServerUsername=`) | Display in UI; sourced from RA login response | Same as token |
| Authenticated flag | `minui.cfg` (`raAuthenticated=`) | UI state | Same as token |
| Hardcore-mode flag | `minui.cfg` (`raHardcoreMode=`) | User preference | Until changed |
| Achievement-set cache | `Saves/.userdata/[platform]/.ra/offline/cache/` | Lets achievements work without an active connection | Until manually deleted or overwritten by a fresh fetch |
| Offline unlock ledger | `Saves/.userdata/[platform]/.ra/offline/ledger.bin` | Append-only, hash-chained durable queue of achievements unlocked while offline (softcore and hardcore) so they sync later; survives crashes and reboots | Compacted on successful sync; fully deleted when no pending entries remain |
| Badge images | `Saves/.userdata/[platform]/.ra/badges/` | Displayed in achievement notifications and lists | Until manually deleted |

**Plaintext storage of the password.** The password is stored
unencrypted on the SD card. NextUI runs on hardware without any secure
enclave, OS-level keychain, or trusted-storage facility; encrypting at
rest with a key also stored on the same SD card would not provide
meaningful protection. Users who do not wish to store a password on the
device can clear the password field after first login — the session
token alone is sufficient for ongoing use until it expires server-side.
Source: `workspace/all/common/config.c` (search `raPassword`).

**Source-of-truth file paths and field names** above are taken directly
from the source: `workspace/all/common/config.c` (config keys),
`workspace/all/common/ra_offline.h` (cache and ledger paths),
`workspace/all/common/ra_badges.h` (badge cache path).

---

## What NextUI sends over the network

When RA is enabled, NextUI sends the following to
`https://retroachievements.org/` (and *only* to that host, for RA
purposes):

| Sent | When | Purpose |
|---|---|---|
| `r=login&u=<username>&p=<password>` | One-time, on initial login | Receive a session token |
| Session token + game hash | On game load | Identify which achievements apply |
| Session token + achievement ID + timestamp + hardcore flag | When you unlock an achievement | Credit the unlock |
| Session token + rich-presence string | Periodically while a game is loaded (~every 2 minutes, by rcheevos default) | Update your "now playing" state on RA |
| User-Agent header on every request | Every request | Identify the client per RA's compliance spec |

The User-Agent NextUI sends is documented in
[`SUBMISSION.md`](./SUBMISSION.md) and contains: NextUI version, host OS
name and kernel version, device platform tag, and the loaded core's
name and version. It does **not** contain a per-installation identifier,
hardware serial, or any other unique device fingerprint.

NextUI does not send any other data to any other server as part of the
RA integration. The Pak Store and Updater contact GitHub release URLs
when the user explicitly invokes them; those are not part of the RA
integration and are governed by GitHub's own privacy policy.

---

## Data controller, processor, and your rights

- **NextUI's role:** NextUI is a software client. The NextUI project
  operates no servers, holds no user database, and has no ability to
  access, retrieve, or delete data on your behalf.
- **RetroAchievements' role:** When you log in, you create or use an
  account on retroachievements.org, a service operated independently of
  the NextUI project. RA is the data controller for any data they
  retain on their servers (achievement progress, leaderboard scores,
  forum posts, etc.). Their privacy policy is the authoritative source
  for what they retain and where:
  <https://retroachievements.org/privacy>.
- **Server location:** RetroAchievements' infrastructure is hosted in
  the United States, per their public privacy policy. Network traffic
  from your device crosses the public internet to reach those servers.
- **Your rights under GDPR and similar regimes:** because NextUI holds
  no data centrally, requests for access, rectification, erasure, or
  portability of data held by NextUI are satisfied by the user
  themselves: delete the relevant files on the SD card, or reflash. For
  data held by RetroAchievements, contact RA directly via their privacy
  policy's stated channels.

---

## Children's data

NextUI is not directed at children under 13. We do not knowingly
collect or process data from children. RetroAchievements has its own
policies regarding minors; consult their privacy policy.

---

## Changes to this policy

This policy is versioned with the NextUI source tree. Changes are made
via pull requests to the NextUI repository and are visible in the
commit history. Significant changes will be summarised in the release
notes for the version that introduces them.

---

## Contact

- **Project repository / issue tracker:**
  <https://github.com/LoveRetro/NextUI/issues>
- **Privacy contact:** [NEEDS CONFIRMATION: maintainer-preferred email
  or "Open an issue tagged `privacy`" — the spec accepts any
  reliably-monitored channel.]
- **Project website:** <https://nextui.loveretro.games>

---

## Verification

Every claim above is grounded in the source. Auditors and reviewers
can confirm:

- Outbound hosts: search the codebase for `https://`. The only RA host
  is `retroachievements.org` (`workspace/all/common/ra_*.c`,
  `ra_integration.c`).
- Stored fields: see `workspace/all/common/config.c` (search
  `fprintf(file, "ra`) for the exhaustive list of persisted keys.
- No telemetry: there are no analytics SDKs, no crash reporters, no
  background uploaders. The HTTP layer (`workspace/all/common/http.c`)
  only sends requests when called explicitly by RA, badge, or
  user-initiated download paths.
