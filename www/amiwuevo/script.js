window.onbeforeunload = function()
{
	console.log("onclose");
	isAlive().then(val => 
		{
			if (val === "1")
				deathSound();
		});

		fetch('/cgi-bin/change_status.sh').then(response => 	
		{
			console.log("");
			if (!response.ok)
				throw new Error("CGI Error");
			window.location.href = "alt.html";
			return (response.text());
		});
};

window.onload = function() 
{
	fetch('/api/name')
	.then(response => {
		if (!response.ok) 
			throw (new Error());
		return (response.text());
	})
	.then(name => 
	{
		// if (name)
			document.getElementById("display-name").textContent = name;
		// else
			// document.getElementById("display-name").textContent = "amiwuevo name";
	})
	.catch(() => {
		document.getElementById("display-name").textContent = "amiwuevo name";
	});

	fetch('/api/alive').then(response => 
	{
		if (!response.ok)
			throw (new Error());
		return (response.text());
	}).then(alive => 
	{
		if (alive)
		{
			if (alive.trim() === '0') 
				window.location.href = "alt.html";
		}
	});

	fetch('/cgi-bin/get_outfit.py').then(response => 
	{
		if (!response.ok)
			throw new Error("CGI Error");
		return (response.text());
	}).then(bg => 
	{
		const imgPath = bg.split('\n').map(line => line.trim()).filter(line => line);
		const outfit = imgPath[imgPath.length - 1];
		document.querySelector('.egg').src = `images/gifs/${outfit}_idle.gif`;
	});
};


// 20260327 Guardar nombre al pulsar Enter
document.addEventListener('DOMContentLoaded', function()
{
	const nameInput = document.getElementById("nameInput");

	if (!nameInput)
		return;

	nameInput.addEventListener("keydown", function(event)
	{
		if (event.key === "Enter")
		{
			event.preventDefault();
			saveName();
		}
	});
});


// 20260307 SAVE NAME
// 20260319 add music play on name save
// 20260325 add alert on name save and error
function saveName()
{
	const name = document.getElementById("nameInput").value;

	if (!name)
	{
		alert("Introduce un nombre");
		return;
	}

	fetch('/api/name', {
		method: 'POST',
		body: name
	})
	.then(response => {

		// Check for 413 Payload Too Large (name too long)
		if (response.status === 413)
		{
			alert("Nombre demasiado largo ❌");
			dialog.showModal();
			return;
		}

		if (response.ok)
		{
			document.getElementById("display-name").textContent = name;
			alert("Nombre guardado");

			const audio = document.getElementById("bg-music");
			audio.play();
		}
		else
		{
			alert("Error al guardar");
		}
	})
	.catch(() => alert("Error de red"));

	dialog.close();
}

// 20260323 CHANGE BACKGROUND (CUSTOM UPLOAD)
document.addEventListener('DOMContentLoaded', function()
{
	
	const btn = document.querySelector('.change-bg-btn');
	if (!btn)
		console.log("btn");
	const fileInput = document.getElementById("bgFileInput");
	if (!fileInput)
		console.log("input");
	const content = document.querySelector('.main-content');
	if (!content)
		console.log("content");

	if (!btn || !fileInput || !content)
		return;

	loadBackground();

	btn.addEventListener("click", function()
	{
		console.log("background button click");
		fetch("/api/background")
		.then(r => r.text())
		.then(path => {

			if (path.includes("images/backgrounds/background_room.png"))
			{
				fileInput.click();
			}
			else
			{
				fetch("/api/background", { method: "DELETE" })
				.then(res => {

					if (!res.ok)
						return;

					loadBackground();
				});
			}
		});
    });

	fileInput.addEventListener("change", function()
	{
		const file = fileInput.files[0];
		if (!file)
			return;

		fetch("/api/background", {
			method: "POST",
			body: file
		})
		.then(res => {

			if (!res.ok)
			{
				alert("Imagen no válida");
				fileInput.value = "";
				return;
			}

			loadBackground();
			fileInput.value = "";
		});
	});
});

 document.addEventListener('DOMContentLoaded', function()
 {
	const btn = document.querySelector('.change-fit-btn');
	if (!btn) 
		return;
	btn.addEventListener('click', function(e) 
	{
		e.preventDefault();
		//Execute chang_outfit.py so it changes the database
		fetch('/cgi-bin/change_outfit.py').then(response => 	
		{
			if (!response.ok)
				throw new Error("CGI Error");
			return (response.text());
		}).then(data => 
		{
			console.log('CGI change_outfit.py output:', data);
			//Execute get_outfit.py so it changes the .css to the file in database

			fetch('/cgi-bin/get_outfit.py').then(response => response.text()).then(bg => 
			{
				const imgPath = bg.split('\n').map(line => line.trim()).filter(line => line);
				const outfit = imgPath[imgPath.length - 1];
				document.querySelector('.egg').src = `images/gifs/${outfit}_idle.gif`;
			});
        });
	});
});

//20260311 CARGAR BACKGROUND (AL INICIO Y DESPUÉS DE SUBIR/ELIMINAR)
function loadBackground()
{
	const content = document.querySelector('.main-content');

	fetch("/api/background")
	.then(r => r.text())
	.then(bg =>
	{
		if (!bg)
			return;
		content.style.backgroundImage = "url('" + bg + "?v=" + Date.now() + "')";
	});
}

//Trigger eating animation with button
document.addEventListener('DOMContentLoaded', function()
{
	const btn = document.querySelector('.trigger-eat-btn');
	if (!btn)
	{
		console.log("Button not found");
		return;
	}
	btn.addEventListener('click', function(e)
	{
		e.preventDefault();
		fetch('/cgi-bin/get_outfit.py').then(response => 
		{
			if (!response.ok)
				throw new Error("CGI Error");
			return (response.text());
		}).then(outfit =>
		{
			outfit = outfit.trim();
			console.log('CGI change_outfit.py output:', outfit);
			const container = document.querySelector('.egg');
			container.src = `images/gifs/${outfit}_eat.gif`;
			setTimeout(() => 
			{
				container.src = `images/gifs/${outfit}_idle.gif`;
			}, 1200);
		});
	});
});

//Dialogue
function readFile() {

	// 20260319 add voice on dialogue trigger
	const voice = document.getElementById("voice-sound"); // obtener el audio de voz
	voice.currentTime = 0; // reiniciar audio para permitir múltiples clicks seguidos
	voice.play(); // reproducir sonido de voz (interacción del usuario → permitido)

	fetch('/api/dialogue')
	.then(response => {
		if (!response.ok) throw new Error();
		return response.text();
	})
	.then(dialogue => {
		if (dialogue)
			document.getElementById("dialogue").textContent = dialogue;
		else
			document.getElementById("dialogue").textContent = "Lore Ipsum";
		fetch('/cgi-bin/get_outfit.py').then(response => 
		{
			if (!response.ok)
				throw new Error("CGI Error");
			return (response.text());
		}).then(outfit =>
		{
			outfit = outfit.trim();
			const container = document.querySelector('.egg');

			container.src = `images/gifs/${outfit}_talk.gif`;

			setTimeout(() =>
			{
				container.src = `images/gifs/${outfit}_idle.gif`;
			}, 2000);
		});
	})
	.catch(() => {
		document.getElementById("dialogue").textContent = "Failed text";
	});
};

// 20260323 KILL AMIWUEVO (UPDATE ALIVE STATE)
document.addEventListener('DOMContentLoaded', function()
{
	const killBtn = document.querySelector('.kill-btn');

	if (!killBtn)
	{
		console.log("killBtn not found");
		return;
	}

	killBtn.addEventListener("click", function(e)
	{
		e.preventDefault();
		isAlive().then(val => 
		{
			if (val === "1")
				deathSound();
		});

		fetch('/cgi-bin/change_status.sh').then(response => 	
		{
			console.log("");
			if (!response.ok)
				throw new Error("CGI Error");
			window.location.href = "alt.html";
			return (response.text());
		});

	});
});

document.addEventListener('DOMContentLoaded', function()
{
	const btn = document.querySelector('.trigger-405');
	if (!btn)
	{
		console.log("Button not found");
		return;
	}
	btn.addEventListener('click', function(e)
	{
		e.preventDefault();
		fetch('/api/name', 
		{ 
			method: 'UPDATE',
			headers: { 'Content-Type': 'application/json' },
			body: JSON.stringify({ key: 'value' })
		}).then(res => 
		{
			if (!res.ok)
			return;
		});;
	});
});

// 20260323 add eat, flush and death sounds (only if alive)
function isAlive()
{
	return fetch('/api/alive')
		.then(res => res.text())
		.then(val => val.trim());
}

function eatSound()
{
	isAlive().then(val => {

		if (val === "0")
			return;

		const eat = document.getElementById("eat-sound");

		if (eat)
		{
			eat.currentTime = 0;
			eat.play();
		}
	});
}

function flushSound()
{
	isAlive().then(val => {

		if (val === "0")
			return;

		const sound = document.getElementById("flush-sound");

		if (sound)
		{
			sound.currentTime = 0;
			sound.play();
		}
	});
}

function deathSound()
{
	const music = document.getElementById("bg-music");
	const death = document.getElementById("death-sound");

	if (music)
		music.pause();

	if (death)
	{
		death.currentTime = 0;
		death.play();
	}
}