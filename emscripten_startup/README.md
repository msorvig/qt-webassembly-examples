
qtloader.js provides API for application life-cycle management: Loading
screen, crash and process exit handling.

There are two use-case variants:
    qtloader_user_managed:       qtloader.js is provided with a container element
                                 and manages status screens and the canvas.
    qtloader_user_external:      the embedding page contains status and canvas elememnts,
                                 and reacts to status updates.
