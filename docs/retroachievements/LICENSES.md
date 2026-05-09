# NextUI Third-Party Components and Licenses

**Last updated:** [NEEDS CONFIRMATION: date of merge]
**Applies to:** NextUI v6.12.0 and later, on Trimui Brick / Smart Pro / Smart Pro S.

This document lists every third-party software component shipped or
linked by NextUI as released, the license that governs it, and the
upstream source. It is published to satisfy the RetroAchievements
compliance spec §F (Transparency and Legality, "Licenses & notices")
and to honour the source-availability obligations of the GPL/LGPL/MPL
components NextUI distributes.

## Compliance posture

NextUI is non-commercial: the project does not sell the software, sell
licenses, sell access, run advertising, or charge for any feature
(see [`MONETIZATION.md`](./MONETIZATION.md)). This posture is what
allows NextUI to ship cores under "no commercial use" terms (FBNeo,
parts of picodrive). Should NextUI ever introduce monetization, those
cores must be removed before that change ships, or written permission
must be obtained from the rightsholders, per the RetroAchievements
spec §F.

NextUI's own source code is licensed GPL-3.0
(see [`LICENSE`](../../LICENSE) at the repository root). Users
exercising their GPL rights to corresponding source should clone the
repository at the tag matching their installed firmware; the build
process is fully described in [`AGENTS.md`](../../AGENTS.md) and the
top-level [`makefile`](../../makefile).

---

## Libretro cores shipped in NextUI v6.12.0

The following cores are bundled as `.pak` archives under
`EXTRAS/Emus/` for both `tg5040` and `tg5050` platforms. Each core is
built from upstream libretro sources at the toolchain pinned in
`toolchains/<platform>-toolchain/`.

| Pak | Core (`<name>_libretro.so`) | Upstream | License |
|---|---|---|---|
| 32X, GG, SEGACD, SG1000, SMS | `picodrive` | <https://github.com/libretro/picodrive> | **Mixed; includes non-commercial components (Cyclone 68k, DrZ80).** Free non-commercial use. [VERIFY: confirm upstream LICENSE at build time] |
| A2600 | `stella2014` | <https://github.com/libretro/stella2014-libretro> | GPL-2.0 [VERIFY] |
| A5200 | `a5200` | <https://github.com/libretro/a5200> | GPL-2.0 [VERIFY] |
| A7800 | `prosystem` | <https://github.com/libretro/prosystem-libretro> | GPL-2.0 [VERIFY] |
| C64, C128, PET, PLUS4, VIC | `vice_x64` / `_x128` / `_xpet` / `_xplus4` / `_xvic` | <https://github.com/libretro/vice-libretro> | GPL-2.0 |
| COLECO | `gearcoleco` | <https://github.com/libretro/gearcoleco> | GPL-3.0 [VERIFY] |
| CPC | `cap32` | <https://github.com/libretro/libretro-cap32> | GPL-3.0 [VERIFY] |
| FBN | `fbneo` | <https://github.com/libretro/FBNeo> | **FBNeo License — non-commercial, no redistribution for profit.** Free non-commercial use. |
| FDS | `fceumm` | <https://github.com/libretro/libretro-fceumm> | GPL-2.0 [VERIFY] |
| LYNX | `handy` | <https://github.com/libretro/libretro-handy> | "Handy License" — free non-commercial use [VERIFY exact terms] |
| MGBA, SGB | `mgba` | <https://github.com/libretro/mgba> | MPL-2.0 |
| MSX | `bluemsx` | <https://github.com/libretro/blueMSX-libretro> | BSD-like (blueMSX license) [VERIFY] |
| NGP, NGPC | `race` | <https://github.com/libretro/RACE> | GPL-2.0 [VERIFY] |
| P8 | `fake08` | <https://github.com/libretro/fake-08> | MIT [VERIFY] |
| PCE | `mednafen_pce_fast` | <https://github.com/libretro/beetle-pce-fast-libretro> | GPL-2.0 |
| PKM | `pokemini` | <https://github.com/libretro/PokeMini> | GPL-3.0 [VERIFY] |
| PRBOOM | `prboom` | <https://github.com/libretro/libretro-prboom> | GPL-2.0 |
| PUAE | `puae2021` | <https://github.com/libretro/libretro-uae> | GPL-2.0 |
| SUPA | `mednafen_supafaust` | <https://github.com/libretro/supafaust> | GPL-2.0 |
| VB | `mednafen_vb` | <https://github.com/libretro/beetle-vb-libretro> | GPL-2.0 |

**[VERIFY] entries** must be confirmed against the actual `LICENSE` file
in the upstream repository at the commit pinned by the NextUI build
toolchain before the RA submission is filed. The toolchain commits are
locked in `toolchains/tg5040-toolchain/` and
`toolchains/tg5050-toolchain/`. See [`SUBMISSION.md`](./SUBMISSION.md)
for the verification checklist.

---

## Frontend libraries

These libraries are linked into the NextUI frontend
(`nextui.elf`, `minarch.elf`, `settings.elf`) at build time. NextUI
either dynamically links them on the device (system-provided) or
statically links a build pulled by the toolchain.

| Library | Purpose | License | Upstream |
|---|---|---|---|
| SDL2 | Windowing, audio, input, threading | Zlib | <https://www.libsdl.org/> |
| libsamplerate | High-quality audio resampling | BSD-2-Clause (since 0.1.9) | <https://github.com/libsndfile/libsamplerate> |
| libzip | Zip archive reading (Pak Store, etc.) | BSD-3-Clause | <https://libzip.org/> |
| libbz2 | bzip2 decompression (rom packs) | bzip2 license (BSD-like) | <https://sourceware.org/bzip2/> |
| liblzma (xz-utils) | xz decompression | Public domain / 0BSD | <https://tukaani.org/xz/> |
| libzstd | Zstandard decompression | BSD-3-Clause | <https://github.com/facebook/zstd> |
| liblz4 | LZ4 decompression | BSD-2-Clause | <https://github.com/lz4/lz4> |
| libchdr | CHD (compressed hard disk) reader | BSD-3-Clause | <https://github.com/rtissera/libchdr> |
| sqlite3 | Game-time tracker, settings | Public domain | <https://www.sqlite.org/> |
| curl | HTTP requests for RA, Pak Store, etc. (subprocess) | curl license (MIT-like) | <https://curl.se/> |
| rcheevos | RetroAchievements client library | MIT | <https://github.com/RetroAchievements/rcheevos> |
| libretro-common | Common helpers used by libretro cores and minarch | MIT | <https://github.com/libretro/libretro-common> |
| BlueZ | Bluetooth audio (system library on device) | GPL-2.0 | <http://www.bluez.org/> |
| libmsettings | Trimui platform settings (vendor library) | [VERIFY] | <https://github.com/shauninman/libmsettings> |

---

## Fonts and assets

| Asset | License | Upstream |
|---|---|---|
| Next font (CJK-capable system font) | [VERIFY: confirm font license — GPL-OFL? SIL Open Font License?] | [VERIFY: upstream URL] |
| Default boot logo, NextUI logo art | All-rights-reserved by contributing artists, granted to the project for use within NextUI; see [`README.md`](../../README.md) for credits ([@SolvalouArt](https://bsky.app/profile/solvalouart.bsky.social) and others) |
| Achievement badge images | Provided by RetroAchievements; sourced from their CDN at runtime; governed by RA's terms |

---

## Tools & build infrastructure (not shipped, but obligation-relevant)

| Tool | License | Upstream |
|---|---|---|
| Docker / cross-compile toolchains (`ghcr.io/loveretro/<platform>-toolchain`) | Sum of base-image OS license + toolchain components — see container manifest | <https://github.com/LoveRetro/tg5040-toolchain> |
| GNU Make, GCC, glibc, etc. | GPL-2.0 / GPL-3.0 / LGPL — distributed by the toolchain image, not by NextUI binaries | upstream OS distribution |

---

## How to obtain corresponding source

For any GPL/LGPL component listed above, the corresponding source is
available via the upstream URL provided in the table, at the commit
captured in the NextUI build toolchains
(`toolchains/<platform>-toolchain/`). For NextUI's own modifications
to those components, see the relevant pull requests and forks linked
from the toolchain repositories.

If a component's upstream becomes unavailable, file an issue at
<https://github.com/LoveRetro/NextUI/issues> with the component name and
NextUI version, and a mirror or archived snapshot will be provided.

---

## Maintenance

Whenever a core is added, removed, or upgraded:

1. Update this file in the same pull request that lands the change.
2. If the core's license changes upstream, re-verify it does not
   conflict with NextUI's non-commercial posture (no monetization, see
   [`MONETIZATION.md`](./MONETIZATION.md)).
3. If a `[VERIFY]` tag remains in this file at the time of any RA
   re-submission, resolve it before submitting.
