package com.example.numbaseball;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {
	public native int openDevice();
	
	public native void startNumbaseball(int fd);
	
	public native void closeDevice(int fd);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		System.loadLibrary("numbaseball");
		
		final int fd = openDevice();		

		Button startButton = (Button) findViewById(R.id.startButton);
		Button ruleButton = (Button) findViewById(R.id.ruleButton);
		startButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Toast.makeText(MainActivity.this, "Game Started!",
						Toast.LENGTH_SHORT).show();
				startNumbaseball(fd);
				closeDevice(fd);
			}
		});

		ruleButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this,
						RuleActivity.class);
				startActivity(intent);
			}
		});
	}
}
