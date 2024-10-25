all:
	em++ requestanimationframe_resume_wasm.cpp -o requestanimationframe_resume_wasm.html -s JSPI
	em++ requestanimationframe_resume_js.cpp -o requestanimationframe_resume_js.html -s JSPI --bind
	em++ eventlistener_resume_js.cpp -o eventlistener_resume_js.html -s JSPI --bind -g
	em++ suspendresumecontroller.cpp -o suspendresumecontroller.html -s JSPI --bind -g
