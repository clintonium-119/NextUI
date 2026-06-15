# NextUI Submission to RetroAchievements — Working Notes

**Status:** Draft, pre-submission.
**Target version:** NextUI v6.12.0
**Spec reference:** [RetroAchievements Hardcore Compliance Requirements](https://docs.retroachievements.org/general/hardcore-compliance-requirements.html)

This is an internal working document used by the NextUI project to
prepare and track its RetroAchievements emulator submission. It
captures every value the submission form will need, every claim we
will make, and the evidence that backs each claim.

This is **not** a published privacy/license/monetization policy — see
[`PRIVACY.md`](./PRIVACY.md), [`LICENSES.md`](./LICENSES.md),
[`MONETIZATION.md`](./MONETIZATION.md) for those.

---

## 1. Submission form values

| Field | Value |
|---|---|
| Emulator name | NextUI |
| Version submitted | 6.12.0 |
| Repository (source) | <https://github.com/LoveRetro/NextUI> |
| Public website | <https://nextui.loveretro.games> |
| Issue tracker | <https://github.com/LoveRetro/NextUI/issues> |
| Maintainer / org | LoveRetro [NEEDS CONFIRMATION: legal/contact name preferred for the RA team] |
| Maintainer contact | [NEEDS CONFIRMATION: email or "Discord: <handle>"] |
| Discord | <https://discord.gg/HKd7wqZk3h> |
| Supported platforms | Trimui Brick, Trimui Smart Pro, Trimui Smart Pro S (`tg5040`, `tg5050`) |
| License | GPL-3.0 |
| Privacy policy URL | <https://github.com/LoveRetro/NextUI/blob/main/docs/retroachievements/PRIVACY.md> |
| Monetization policy URL | <https://github.com/LoveRetro/NextUI/blob/main/docs/retroachievements/MONETIZATION.md> |
| Licenses page URL | <https://github.com/LoveRetro/NextUI/blob/main/docs/retroachievements/LICENSES.md> |

---

## 2. User-Agent (spec §C)

NextUI v6.12.0 emits a User-Agent in the form:

```
NextUI/v<NEXTUI_VERSION> (<OS> <kernel>; <platform>) [<core>/<core_version>]
```

- `<NEXTUI_VERSION>` is read from the `VERSION` file at the repo root,
  validated as a dotted three-segment numeric version at build time.
- `<OS>` is `Linux` (from `uname(2)`).
- `<kernel>` is the kernel version trimmed to numeric+dot characters
  (e.g. `6.6.0-arch1-1` → `6.6.0`) so the OS clause stays numeric per
  spec §C examples.
- `<platform>` is the device profile (`tg5040`, `tg5050`, or `desktop`
  for development builds) — preserved as a sub-clause, not as the OS.
- The core clause is appended only when a core is loaded (the
  retro_get_system_info name and version, registered via
  `HTTP_setCoreInfo()`), and removed on core unload.

### Sample UAs (verifiable by tcpdump or RA server logs)

| Context | UA |
|---|---|
| Idle / login from Settings.pak | `NextUI/v6.12.0 (Linux 6.6.0; tg5040)` |
| In-game, snes9x core | `NextUI/v6.12.0 (Linux 6.6.0; tg5040) Snes9x/1.62.3` |
| Desktop development build | `NextUI/v6.12.0 (Linux 6.x; desktop)` |

### Source files

- Construction: `workspace/all/common/http.c` (`HTTP_getUserAgent`,
  `HTTP_setCoreInfo`, `http_get_os_clause`).
- Core registration: `workspace/all/minarch/ma_core.c` (in `Core_open`
  after `core.get_system_info()`, and cleared in `Core_quit`).
- Version source: `VERSION` (repo root); injected via
  `-DNEXTUI_VERSION=` from `makefile`, `workspace/all/minarch/makefile`,
  `workspace/all/settings/makefile`.

---

## 3. Fork lineage disclosure (spec §C)

NextUI is a fork of [MinUI](https://github.com/shauninman/MinUI) by
Shaun Inman. We are disclosing this voluntarily as a transparency
measure; the spec's specific §C trigger ("inheriting a default user
agent from a forked codebase") does **not** apply to NextUI because
the parent project does not contain a RetroAchievements integration
and never emitted a User-Agent identifying as anything. NextUI's RA
integration is entirely original to this project and has always
identified as `NextUI/...` on the wire.

The MinUI fork relationship is plainly stated in the project
[`README.md`](../../README.md) ("A CFW based of MinUI…") and is
preserved in the public Git history.

---

## 4. Hardcore mode enforcement — restriction matrix (spec §B)

This matrix demonstrates what is blocked in hardcore mode and how each
block is enforced. Every row names the module and function so reviewers
can audit the source. (`minarch.c` was refactored into `ma_*.c` modules;
these paths reflect the current layout.)

| Restriction | Enforced where | Notification user sees |
|---|---|---|
| Load state | `ma_saves.c` `State_read` returns early before deserialising | "Load states disabled in Hardcore mode" |
| Save state (create) | **Intentionally allowed** (`ma_saves.c` `State_write`, ungated) — see note below | — |
| Auto-resume on next boot | `ma_saves.c` `State_resume` silently consumes the resume marker | (no nag — see Phase 1 design notes) |
| Auto-resume marker write | `ma_menu.c` `Menu_beforeSleep` skips `putFile(AUTO_RESUME_PATH…)` | — |
| Quick-resume / Game Switcher save-on-exit | `ma_input.c` `SHORTCUT_SAVE_QUIT`, `SHORTCUT_GAMESWITCHER` skip `Menu_saveState` | (game returns to launcher; relaunch boots ROM cleanly) |
| Cheat enable (toggle) | `ma_frontend_opts.c` `OptionCheats_optionChanged` reverts the toggle | "Cheats disabled in Hardcore mode" |
| Cheat auto-load on game start | `ma_core.c` `Core_applyCheats` skips per-entry `cheat_set` | (one-time notification per game-load if any cheat was disabled) |
| Cheat persistent enable flag | `ma_cheats.c` `Cheats_load` force-clears all `enabled` flags on load | (notification as above) |
| Rewind: hold-rewind hotkey | `ma_input.c` `SHORTCUT_HOLD_REWIND` gated | "Rewind disabled in Hardcore mode" |
| Rewind: toggle-rewind hotkey | `ma_input.c` `SHORTCUT_TOGGLE_REWIND` gated | "Rewind disabled in Hardcore mode" |
| Rewind: buffer allocation | `ma_rewind.c` `Rewind_init` skipped — no memory allocated, no worker thread | — |
| Slow-mo / frame advance / TAS / RAM editor | None present in NextUI; nothing to gate | — |

**Save vs. load — the compliance line is at loading, not saving.** The spec
draws the line explicitly:

> "Users should be allowed to create save states while in hardcore for
> debugging purposes, but those states cannot be loadable when hardcore mode
> is enabled."
> "Loading save states is ALWAYS blocked in hardcore."

Accordingly, NextUI **permits creating** save states in hardcore (`State_write`
has no hardcore guard) and **unconditionally blocks loading** them
(`State_read` returns before deserialising, regardless of connectivity). There
is no exploit path: a state created in hardcore cannot be loaded while hardcore
is active, and a user who drops to softcore to load it earns only softcore
credit. The sole save-state item in the spec's Auto-Fail Criteria — "Loading
save states in hardcore mode" — is therefore satisfied.

### Toggle behaviour and visibility (spec §C, §D)

- The hardcore toggle lives in **Settings → RetroAchievements →
  Hardcore Mode** (separate `settings.elf` reachable via Tools →
  Settings, not in the in-game pause menu).
- Toggling is a write to `minui.cfg`. Because the Settings app is a
  separate process and no game can be loaded while it is open, no
  mid-session transitions originate from the user-facing toggle.
- Mid-session transitions occur only when **softcore is elevated to
  hardcore on the first successful network connection after a
  cold-boot in offline mode**: a hardcore-preferring user who boots
  the device with no network is started in softcore (per spec §B —
  "online required to start a hardcore session"), and the elevation
  only takes effect once connectivity is established. At that point
  NextUI invokes `Hardcore_onEnableTransition()` which clears all
  cheat enabled flags, calls `core.cheat_reset()`, frees the rewind
  buffer, and calls `core.reset()`. The user sees a notification that
  hardcore mode is now active and the game has been reset.

### Connectivity loss during an active hardcore session

NextUI follows the rcheevos reference design: a transient network drop
during a hardcore session **does not** demote the session to softcore.
- `rc_client_set_hardcore_enabled` is **not** flipped on
  `RC_CLIENT_EVENT_DISCONNECTED`. The user remains in hardcore.
- Unlocks earned during the drop are written to the offline ledger
  (`Saves/.userdata/[platform]/.ra/offline/ledger.bin`) with the
  hardcore flag set, durable across crashes and reboots.
- The existing in-flight gate in `ra_server_call` suppresses rcheevos'
  own in-memory retry queue while the ledger holds the entry,
  preventing double submission.
- On `RC_CLIENT_EVENT_RECONNECTED` NextUI does **not** reset the game;
  it simply clears offline state and starts the sync engine, which
  POSTs each queued unlock to `r=awardachievement` with the original
  `h=1` flag and an `o=<seconds_since>` offset reflecting the original
  unlock timestamp. The server's response — `SYNC_ACK` in our ledger —
  clears the entry from the pending cache and patches the
  `startsession` cache for the next launch.
- **Cross-session demotion.** RA's offline contract is explicit:
  *"if you close the emulator before you get internet back then your
  achievements won't sync, and if you close the emulator and later
  reopen it, it will NOT unlock those achievements for you once
  connected again."* NextUI honours this for hardcore credit
  specifically. Pending unlocks whose ledger timestamp predates the
  current session's `SESSION_START` (recorded via
  `RA_Sync_setCurrentSessionStart` on game load) are submitted with
  `h=0` regardless of their original record. The user is notified
  (two sequential toasts: `"N Hardcore unlock(s) synced as Softcore"`
  followed by `"Request manual unlock for Hardcore credit"`) and
  directed to RetroAchievements to recover hardcore credit through
  the site's manual-unlock process. The ledger record itself keeps
  `hardcore=1` for audit fidelity. This matches the existing
  quick-resume rule ("resumed session must drop to Softcore")
  generalised to all session boundaries.
- **Settings.pak sync path.** The standalone Settings app (Tools →
  Settings → RetroAchievements → Sync Offline Unlocks) runs as a
  separate process from minarch and never registers a SESSION_START.
  By the rule above this means every successful hardcore unlock
  submitted from Settings.pak is cross-session by definition and is
  demoted to softcore — followed by a dismissable overlay directing
  the user to contact RetroAchievements for a manual unlock if they
  want hardcore credit reinstated. This is intentional: any hardcore
  unlock reaching the Settings.pak sync UI was earned in a prior
  minarch process that didn't sync before exit, exactly the scenario
  RA's FAQ disallows for automatic hardcore credit.
- Rationale: save-state load is already blocked in hardcore regardless
  of connection state, so a wifi drop cannot be used to cheese
  hardcore. Forcing softcore on every blip would silently downgrade
  diligent hardcore players on flaky WiFi (the realistic case on
  handhelds), which is the worse failure mode. This mirrors the
  intentional RetroArch behaviour (see
  [libretro/RetroArch#15574](https://github.com/libretro/RetroArch/issues/15574)).
- The user sees an `"Offline — unlocks will sync on reconnect"`
  notification on the first drop of a session, and a
  `"Reconnected — syncing pending unlocks"` notification when the
  network returns (only if the drop notification was shown, so brief
  blips are silent).
- Visible mode indication on every game start: the existing
  `ra_show_game_summary` notification is appended with
  `· Hardcore` or `· Softcore` per spec §C ("normally done when the
  player starts a game — they can see a brief informational message
  saying what mode they're in").
- The pause-menu Achievements row description and the achievement-list
  status header are also tagged with `· Hardcore` in hardcore mode.

---

## 5. Cores shipped (spec §F licensing)

Full table in [`LICENSES.md`](./LICENSES.md). Quick summary for the
submission narrative:

- **30 libretro cores** bundled across both supported platforms.
- **Mostly GPL-2.0** (Mednafen-derived, VICE, fceumm, etc.).
- **Two non-commercial-licensed cores** ship: FBNeo (FBN.pak) and
  picodrive (used by 32X, GG, SEGACD, SG1000, SMS paks). Permitted
  under spec §F because NextUI is non-commercial; see
  [`MONETIZATION.md`](./MONETIZATION.md).
- **No closed-source or pirated assets** ship with NextUI. BIOS files
  required by some cores are user-supplied to the `Bios/` directory.

---

## 6. Pre-submission verification checklist

Maintainer should walk through this list once before filing:

- [ ] `VERSION` matches the imminent release tag (`6.12.0` at time of
      writing).
- [ ] `git tag v6.12.0` exists and points at the same commit as the
      submission build.
- [ ] All `[NEEDS CONFIRMATION]` and `[VERIFY]` markers in
      `docs/retroachievements/*.md` and the README RA section are
      resolved.
- [ ] `PRIVACY.md` privacy contact is a real, monitored channel.
- [ ] `LICENSES.md` core license entries have all `[VERIFY]` tags
      resolved against the upstream `LICENSE` file at the toolchain's
      pinned commit.
- [ ] Demo material captured (see §7 below).
- [ ] Build artefact for the submission carries the exact UA documented
      in §2 (verify by `tcpdump -A 'host retroachievements.org'` on a
      desktop dev build, or by capturing the request in RA server logs).
- [ ] Hardcore restrictions tested end-to-end on a real device against a
      real RA account: load-state/rewind/cheats blocked, save-state
      allowed, indicator shown, and *softcore* unlocks register. (A live
      *hardcore* unlock cannot register until RA whitelists the
      User-Agent — that is a post-approval smoke test, not a pre-file
      gate. Pre-approval, hardcore play earns softcore + shows the
      "Unknown Emulator" warning, ID 101000001.)

---

## 7. Demo material for submission

The RA submission process expects either screen captures or a short
video showing each restriction in action. Suggested clips, each ≤30
seconds:

1. **Hardcore mode toggle:** Tools → Settings → RetroAchievements →
   Hardcore Mode → On.
2. **Game-start notification:** launch a known-supported game; capture
   the `· Hardcore` tag in the welcome notification.
3. **Load state blocked:** Menu → Save Changes → Load State while in
   hardcore — show notification.
4. **Cheats blocked:** Menu → Cheats → toggle a cheat in hardcore —
   show notification.
5. **Rewind blocked:** assigned rewind hotkey while in hardcore — show
   notification.
6. **Quick-resume cleared:** in hardcore, Save & Quit a game; relaunch
   it — show that it boots from the ROM start, not a saved state.
7. **Cold-boot offline elevation:** boot the device with WiFi off
   while hardcore preference is set; observe the game starts in
   softcore. Enable WiFi → first reconnect triggers elevation +
   `core.reset()` + the "Reconnected — Hardcore mode ON (game reset)"
   notification.
8. **Mid-session drop in hardcore (within-session sync):** start a
   hardcore session online, disable WiFi mid-play, earn an achievement
   during the offline window, re-enable WiFi **without exiting the
   game**. Expected: "Offline — unlocks will sync on reconnect"
   notification on drop; achievement appears in the achievement list
   with the "Pending sync" indicator; on reconnect, "Reconnected —
   syncing pending unlocks" notification and the unlock appears on
   retroachievements.org as a **hardcore** unlock with the correct
   original timestamp.
9. **Cross-session demotion (hardcore → softcore):** repeat step 8 but
   *exit the game* (or kill the app, or reboot the device) before
   reconnecting. Reopen the game with WiFi available. Expected: sync
   engine submits the pending unlock as **softcore**, the user sees
   the two demotion toasts ("N Hardcore unlock(s) synced as Softcore"
   followed by "Request manual unlock for Hardcore credit"), and the
   achievement appears on retroachievements.org as a softcore unlock.
   Repeat once via Tools → Settings → RetroAchievements → Sync
   Offline Unlocks instead of re-entering the game; expected: same
   outcome, plus the Settings overlay directing the user to contact
   RetroAchievements for a manual unlock.
10. **An actual hardcore unlock** appearing on retroachievements.org
    for the test account.

---

## 8. Known caveats to disclose proactively

- **Auto-resume markers:** if the user toggles hardcore *while a
  resume marker exists* (created during a previous softcore session),
  the marker is silently consumed on next launch (no auto-load
  performed, no notification). This is intentional — see
  [`HARDCORE_MODE.md`](../../HARDCORE_MODE.md) Phase 1.4 design notes
  — and avoids spamming a load-blocked notification on every hardcore
  game launch.
- **Credential storage:** the RA password is never written to disk — it
  is held in memory only during login, exchanged for a session token,
  then discarded (and a legacy plaintext `raPassword=` line from an older
  build is purged on first launch). Only the session token is persisted;
  it is stored unencrypted and is bearer-equivalent, standard for
  handheld emulators without secure-element hardware. The token is
  redacted from the settings debug dump, and the user can log out to
  clear it. Documented in [`PRIVACY.md`](./PRIVACY.md) §"What NextUI
  stores locally".
- **Rich Presence in pause menu:** rcheevos's `rc_client_idle()` is
  driven from the same loop that feeds the pause menu, so RP pings
  continue while the menu is open. [VERIFY: confirm by inspecting
  rcheevos worker behaviour during long pause-menu sessions before
  submission.]
- **Encore Mode and Challenge Indicators (shipped, fairness-neutral):**
  NextUI exposes RA's Encore Mode (Settings → RetroAchievements →
  Encore Mode) via `rc_client_set_encore_mode_enabled`. Re-triggering an
  already-earned achievement grants no new server credit — the server
  reports it as already owned — and a genuinely new unlock during encore
  play still obeys the normal hardcore restrictions. A separate "Show
  Challenge Indicators" toggle displays a lower-right trophy pill and
  pins active-challenge rows in the achievement list, driven purely by
  rcheevos's `CHALLENGE_INDICATOR_SHOW/HIDE` events; it is display-only
  and confers no gameplay advantage. Neither feature relaxes a hardcore
  restriction.
