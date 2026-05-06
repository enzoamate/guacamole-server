# Oklavier guacamole-server fork

This is a downstream fork of [Apache Guacamole Server](https://github.com/apache/guacamole-server),
maintained by the Oklavier project (https://github.com/enzoamate/oklavier) to add support for
RDP virtual channels that upstream guacd does not yet implement:

- **URBDRC** (USB redirection) — tracking [GUACAMOLE-522](https://issues.apache.org/jira/browse/GUACAMOLE-522)
  and [PR #610](https://github.com/apache/guacamole-server/pull/610). Until upstream merges, our
  patches add a Guacamole-protocol bridge for `[MS-RDPEUSB]`.
- **rdpecam** (camera redirection) — no upstream issue. Implements
  `[MS-RDPECAM]` so browsers (via `getUserMedia`) can stream a webcam into the remote Windows
  session.

## Branch layout

- `main` — tracks upstream `apache/guacamole-server@main`. We try to keep it 0-delta.
- `oklavier-channels` — our active patch branch. All Oklavier-specific changes land here.
  Rebased on top of upstream periodically.

## Licensing

This fork is published under the Apache License 2.0, the same license as upstream.
Upstream `LICENSE`, `NOTICE`, and copyright notices are preserved verbatim.

## Container image

The Oklavier project builds container images from this branch and publishes them as
`ghcr.io/enzoamate/oklavier-guacd:<tag>`. The Oklavier Helm chart points to those
images by default; upstream `guacamole/guacd` images can be substituted by anyone who
does not need the patched channels.
