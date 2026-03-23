
// Cargar nombre al iniciar
window.onload = function() 
{
	fetch('/api/name')
	.then(response => {
		if (!response.ok) throw new Error();
		return response.text();
	})
	.then(name => {
		if (name)
			document.getElementById("display-name").textContent = name;
		else
			document.getElementById("display-name").textContent = "amiwuevo name";
	})
	.catch(() => {
		document.getElementById("display-name").textContent = "amiwuevo name";
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

// 20260307 SAVE NAME
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
		if (response.ok) {
			document.getElementById("display-name").textContent = name;
			alert("Nombre guardado");
		} else {
			alert("Error al guardar");
		}
	})
	.catch(() => alert("Error de red"));
	dialog.close()
}

// 20260311 CHANGE BACKGROUND (CUSTOM UPLOAD)
document.addEventListener('DOMContentLoaded', function()
{
	
	const btn = document.querySelector('.change-bg-btn');
	if (!btn)
		console.log("btn");
	const fileInput = document.getElementById("bgFileInput");
	if (!fileInput)
		console.log("input");
	const content = document.querySelector('.content');
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

			// si es fondo por defecto → subir imagen
			if (path.includes("images/backgrounds/bg_01.png"))
			{
				fileInput.click();
			}
			// si hay imagen subida → borrar
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
				return;
			}

			loadBackground();
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
	const content = document.querySelector('.content');

	fetch("/api/background")
	.then(r => r.text())
	.then(bg => {

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
		console.log("Button ot found");
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