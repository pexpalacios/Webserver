// Cargar nombre al iniciar
window.onload = function() 
{
	fetch('/api/name')
	.then(response => {
		if (!response.ok) 
			throw (new Error());
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
};

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

			if (path.includes("images/backgrounds/background.jpg"))
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
		console.log("Outfit button clicked");
	});
});

//20260311 CARGAR BACKGROUND (AL INICIO Y DESPUÉS DE SUBIR/ELIMINAR)
function loadBackground()
{
	const content = document.querySelector('.main-content');

	fetch("/api/background")
	.then(r => r.text())
	.then(bg => {

		if (!bg)
			return;

		content.style.backgroundImage = "url('" + bg + "?v=" + Date.now() + "')";
	});
}

//405 button
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