# NextUI Monetization Policy — RetroAchievements Submission

**Last updated:** 2026-06-09
**Applies to:** NextUI v6.12.0 and later.

NextUI is free and open-source software, distributed under the
GNU General Public License v3.0 (see [`LICENSE`](../../LICENSE) at the
repository root).

## Feature matrix

| Feature | Free | Paid |
|---|:---:|:---:|
| Core emulation engine | ✅ | — |
| Save states | ✅ | — |
| Rewind | ✅ | — |
| Cheats | ✅ | — |
| Shaders, overlays, scaling, color-temp, brightness | ✅ | — |
| Game switcher / quick resume | ✅ | — |
| WiFi, Bluetooth audio, NTP time sync | ✅ | — |
| RetroAchievements integration (login, unlocks, leaderboards, hardcore) | ✅ | — |
| Pak Store, Updater, all community Paks | ✅ | — |
| Cloud saves | ❌ (not offered) | ❌ (not offered) |
| Cloud-synced achievements beyond what RA provides | ❌ (not offered) | ❌ (not offered) |
| Anything else | ✅ if it ships | — |

**There are no paid features in NextUI. There are no in-app purchases,
no subscriptions, no advertising, no upsells, and no "premium"
unlocks.** Every feature shipped in the firmware is available to every
user at no cost.

## Why this matters for the RA submission

The RetroAchievements compliance spec requires that any monetized
emulator publish a free-vs-paid feature matrix and disclose any limits
on the free tier (e.g., cloud-save quotas, ad-supported sessions). The
spec further forbids paid features that undermine hardcore enforcement
(e.g., a "skip to credits" button gated behind a paywall).

NextUI ships nothing of the kind. The matrix above is published to
satisfy the disclosure requirement explicitly, even though every row
that could be paid is "not offered."

## Donations

The NextUI project [NEEDS CONFIRMATION: link or remove this section
depending on whether the maintainer accepts donations and via which
platform — GitHub Sponsors, Open Collective, Ko-fi, etc.] accepts
voluntary donations. Donations confer no access to features, no
priority support, no influence over roadmap, and no in-game benefits
of any kind. They are gratitude payments, not purchases.

## Third-party Paks

NextUI's Pak Store distributes community-built extensions (`.pakz`).
Each Pak is independently authored and licensed; NextUI does not
process payments on behalf of Pak authors. Whether an individual Pak
is free or paid is the responsibility of its author. **No Pak in the
official Pak Store catalogue is paid as of NextUI v6.12.0.** [NEEDS
CONFIRMATION: confirm with maintainer that this remains true at
submission time.]

## Changes to this policy

If NextUI ever introduces a paid feature, this matrix must be updated
in the same change that introduces the feature, and the
RetroAchievements team must be notified before the change ships in a
release. Compliance is a continuous obligation, not a launch-day box.
