# NextUI Submission to RetroAchievements — Working Notes

**Status:** Draft, pre-submission.
**Target version:** NextUI v6.12.0
**Spec reference:** [RetroAchievements Hardcore Compliance Specification](https://retroachievements.org/game/24181/hashes) <!-- [NEEDS CONFIRMATION: replace with actual spec URL referenced in HARDCORE_MODE.md] -->

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
- Core registration: `workspace/all/minarch/minarch.c` (after
  `core.get_system_info()` and in `Core_quit`).
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
block is enforced. Every row is mapped to one or more file:line
references for reviewers who wish to audit the source.

| Restriction | Enforced where | Notification user sees |
|---|---|---|
| Load state | `workspace/all/minarch/minarch.c` `State_read` | "Load states disabled in Hardcore mode" |
| Save state | Allowed (per spec §B note: saving is not a hardcore violation) | — |
| Auto-resume on next boot | `State_resume` silently consumes the resume marker | (no nag — see Phase 1 design notes) |
| Auto-resume marker write | `Menu_beforeSleep` skipped | — |
| Quick-resume / Game Switcher save-on-exit | `SHORTCUT_SAVE_QUIT`, `SHORTCUT_GAMESWITCHER` skip `Menu_saveState` | (game returns to launcher; relaunch boots ROM cleanly) |
| Cheat enable (toggle) | `workspace/all/minarch/minarch.c` cheat-toggle path | "Cheats disabled in Hardcore mode" |
| Cheat auto-load on game start | `Core_applyCheats` skips per-entry `cheat_set` | (one-time notification per game-load if any cheat was disabled) |
| Cheat persistent enable flag | `parse_cheats` force-clears all `enabled` flags on load | (notification as above) |
| Rewind: hold-rewind hotkey | `SHORTCUT_HOLD_REWIND` gated | "Rewind disabled in Hardcore mode" |
| Rewind: toggle-rewind hotkey | `SHORTCUT_TOGGLE_REWIND` gated | "Rewind disabled in Hardcore mode" |
| Rewind: buffer allocation | `Rewind_init` skipped — no memory allocated, no worker thread | — |
| Slow-mo / frame advance / TAS / RAM editor | None present in NextUI; nothing to gate | — |

### Toggle behaviour and visibility (spec §C, §D)

- The hardcore toggle lives in **Settings → RetroAchievements →
  Hardcore Mode** (separate `settings.elf` reachable via Tools →
  Settings, not in the in-game pause menu).
- Toggling is a write to `minui.cfg`. Because the Settings app is a
  separate process and no game can be loaded while it is open, no
  mid-session transitions originate from the user-facing toggle.
- Mid-session transitions occur only on **reconnect after offline
  play**: when the device regains network access while a game is
  loaded with the user's persisted hardcore preference set, NextUI
  invokes `Hardcore_onEnableTransition()` which clears all cheat
  enabled flags, calls `core.cheat_reset()`, frees the rewind buffer,
  and calls `core.reset()`. The user sees a notification that hardcore
  mode is now active and the game has been reset.
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
- [ ] Hardcore mode tested end-to-end on a real device against a real
      RA account — at least one hardcore unlock confirmed live in the
      RA dashboard.

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
7. **Reconnect transition:** start in offline (hardcore preference set,
   client offline → softcore active); reconnect → show notification +
   game reset.
8. **An actual hardcore unlock** appearing on retroachievements.org for
   the test account.

---

## 8. Known caveats to disclose proactively

- **Auto-resume markers:** if the user toggles hardcore *while a
  resume marker exists* (created during a previous softcore session),
  the marker is silently consumed on next launch (no auto-load
  performed, no notification). This is intentional — see
  [`HARDCORE_MODE.md`](../../HARDCORE_MODE.md) Phase 1.4 design notes
  — and avoids spamming a load-blocked notification on every hardcore
  game launch.
- **Plaintext password storage:** documented in
  [`PRIVACY.md`](./PRIVACY.md) §"What NextUI stores locally". Standard
  for handheld emulators without secure-element hardware; the user can
  clear the password field after first login.
- **Rich Presence in pause menu:** rcheevos's `rc_client_idle()` is
  driven from the same loop that feeds the pause menu, so RP pings
  continue while the menu is open. [VERIFY: confirm by inspecting
  rcheevos worker behaviour during long pause-menu sessions before
  submission.]
