#!/usr/bin/env python3
"""Fills i18n/ubdocscan_<lang>.ts with the translations below.

Workflow when texts change:
  1. lupdate (see README, "Translations") to refresh the .ts files
  2. add the new English texts to every language below
  3. python3 i18n/translate.py   - fails and lists any text still missing

English is the source language; its file only carries the plural forms of
the %n texts. Plural texts take a list: [singular, plural].
"""
import sys
import xml.etree.ElementTree as ET

EN = {
    "%n page(s)": ["%n page", "%n pages"],
    "Continue scan (%n page(s))": ["Continue scan (%n page)", "Continue scan (%n pages)"],
}

DE = {
    "Back": "Zurück",
    "Appearance": "Darstellung",
    "Like the phone": "Wie das Telefon",
    "Light": "Hell",
    "Dark": "Dunkel",
    "Language": "Sprache",
    "Settings": "Einstellungen",
    "Text recognition: Tesseract OCR. The languages for it are chosen when saving a PDF.":
        "Texterkennung: Tesseract OCR. Die Sprachen dafür wählst du beim Speichern eines PDFs.",
    # --- home -------------------------------------------------------------
    "%1 KB": "%1 KB",
    "%1 MB": "%1 MB",
    "Continue scan (%n page(s))": ["Scan fortsetzen (%n Seite)", "Scan fortsetzen (%n Seiten)"],
    "New scan": "Neuer Scan",
    "Saved documents": "Gespeicherte Dokumente",
    "No documents yet. Tap \"New scan\" to scan your first one.":
        "Noch keine Dokumente. Tippe auf „Neuer Scan“, um dein erstes zu scannen.",
    "Open or share...": "Öffnen oder teilen...",
    "Open or share with": "Öffnen oder teilen mit",
    "Rename": "Umbenennen",
    "Delete": "Löschen",
    "Rename document": "Dokument umbenennen",
    "A document with this name already exists.": "Ein Dokument mit diesem Namen gibt es schon.",
    "Delete document": "Dokument löschen",
    "Delete \"%1\"? This cannot be undone.":
        "„%1“ löschen? Das kann nicht rückgängig gemacht werden.",
    # --- scanning ---------------------------------------------------------
    "New document": "Neues Dokument",
    "Discard": "Verwerfen",
    "Discard document": "Dokument verwerfen",
    "Delete all scanned pages of this document?": "Alle gescannten Seiten dieses Dokuments löschen?",
    "Add pages with the camera or import a photo.":
        "Füge Seiten mit der Kamera hinzu oder importiere ein Foto.",
    "Camera": "Kamera",
    "Import": "Importieren",
    "Import a photo from": "Foto importieren aus",
    "Edit corners and filter": "Ecken und Filter bearbeiten",
    "Rotate": "Drehen",
    "Move earlier": "Nach vorne verschieben",
    "Move later": "Nach hinten verschieben",
    "Delete page": "Seite löschen",
    "The photo could not be taken.": "Das Foto konnte nicht aufgenommen werden.",
    "The camera is not available: %1": "Die Kamera ist nicht verfügbar: %1",
    "Hold the phone above the page": "Halte das Telefon über die Seite",
    "Flash on": "Blitz an",
    "Flash off": "Blitz aus",
    "Take photo": "Foto aufnehmen",
    "Adjust page": "Seite anpassen",
    "Edit page %1": "Seite %1 bearbeiten",
    "Whole photo": "Ganzes Foto",
    "Use page": "Seite übernehmen",
    "Colour": "Farbe",
    "Grey": "Grau",
    "Black & white": "Schwarzweiß",
    "Processing page...": "Seite wird bearbeitet...",
    "The photo could not be opened.": "Das Foto konnte nicht geöffnet werden.",
    "The page could not be saved.": "Die Seite konnte nicht gespeichert werden.",
    # --- saving -----------------------------------------------------------
    "Save as PDF": "Als PDF speichern",
    "Saved \"%1\"": "„%1“ gespeichert",
    "Title": "Titel",
    "Scan %1": "Scan %1",
    "%n page(s)": ["%n Seite", "%n Seiten"],
    "Recognise text (OCR)": "Text erkennen (OCR)",
    "The PDF gets an invisible text layer: you can search it and copy text from it.":
        "Das PDF erhält eine unsichtbare Textebene: Du kannst darin suchen und Text daraus kopieren.",
    "The PDF contains the page images only.": "Das PDF enthält nur die Seitenbilder.",
    "Languages in the document": "Sprachen im Dokument",
    "Save": "Speichern",
    "Recognising text: page %1 of %2": "Texterkennung: Seite %1 von %2",
    "Writing PDF: page %1 of %2": "PDF wird geschrieben: Seite %1 von %2",
    "This takes a few seconds per page.": "Das dauert einige Sekunden pro Seite.",
    "English": "Englisch",
    "German": "Deutsch",
    "French": "Französisch",
    "Spanish": "Spanisch",
    "Italian": "Italienisch",
    "The text recognition could not be started (%1).":
        "Die Texterkennung konnte nicht gestartet werden (%1).",
    "The PDF file could not be created.": "Die PDF-Datei konnte nicht erstellt werden.",
    "Page %1 could not be read.": "Seite %1 konnte nicht gelesen werden.",
    "The text on page %1 could not be recognised.":
        "Der Text auf Seite %1 konnte nicht erkannt werden.",
    "The PDF file could not be written.": "Die PDF-Datei konnte nicht geschrieben werden.",
}

FR = {
    "Back": "Retour",
    "Appearance": "Apparence",
    "Like the phone": "Comme le téléphone",
    "Light": "Clair",
    "Dark": "Sombre",
    "Language": "Langue",
    "Settings": "Paramètres",
    "Text recognition: Tesseract OCR. The languages for it are chosen when saving a PDF.":
        "Reconnaissance du texte : Tesseract OCR. Les langues se choisissent lors de "
        "l'enregistrement d'un PDF.",
    # --- home -------------------------------------------------------------
    "%1 KB": "%1 Ko",
    "%1 MB": "%1 Mo",
    "Continue scan (%n page(s))": ["Continuer le scan (%n page)", "Continuer le scan (%n pages)"],
    "New scan": "Nouveau scan",
    "Saved documents": "Documents enregistrés",
    "No documents yet. Tap \"New scan\" to scan your first one.":
        "Aucun document pour l'instant. Touchez « Nouveau scan » pour numériser le premier.",
    "Open or share...": "Ouvrir ou partager...",
    "Open or share with": "Ouvrir ou partager avec",
    "Rename": "Renommer",
    "Delete": "Supprimer",
    "Rename document": "Renommer le document",
    "A document with this name already exists.": "Un document portant ce nom existe déjà.",
    "Delete document": "Supprimer le document",
    "Delete \"%1\"? This cannot be undone.": "Supprimer « %1 » ? Cette action est irréversible.",
    # --- scanning ---------------------------------------------------------
    "New document": "Nouveau document",
    "Discard": "Abandonner",
    "Discard document": "Abandonner le document",
    "Delete all scanned pages of this document?":
        "Supprimer toutes les pages numérisées de ce document ?",
    "Add pages with the camera or import a photo.":
        "Ajoutez des pages avec la caméra ou importez une photo.",
    "Camera": "Caméra",
    "Import": "Importer",
    "Import a photo from": "Importer une photo depuis",
    "Edit corners and filter": "Modifier les coins et le filtre",
    "Rotate": "Pivoter",
    "Move earlier": "Déplacer avant",
    "Move later": "Déplacer après",
    "Delete page": "Supprimer la page",
    "The photo could not be taken.": "Impossible de prendre la photo.",
    "The camera is not available: %1": "La caméra n'est pas disponible : %1",
    "Hold the phone above the page": "Tenez le téléphone au-dessus de la page",
    "Flash on": "Flash activé",
    "Flash off": "Flash désactivé",
    "Take photo": "Prendre une photo",
    "Adjust page": "Ajuster la page",
    "Edit page %1": "Modifier la page %1",
    "Whole photo": "Photo entière",
    "Use page": "Utiliser la page",
    "Colour": "Couleur",
    "Grey": "Gris",
    "Black & white": "Noir et blanc",
    "Processing page...": "Traitement de la page...",
    "The photo could not be opened.": "Impossible d'ouvrir la photo.",
    "The page could not be saved.": "Impossible d'enregistrer la page.",
    # --- saving -----------------------------------------------------------
    "Save as PDF": "Enregistrer en PDF",
    "Saved \"%1\"": "« %1 » enregistré",
    "Title": "Titre",
    "Scan %1": "Scan %1",
    "%n page(s)": ["%n page", "%n pages"],
    "Recognise text (OCR)": "Reconnaître le texte (OCR)",
    "The PDF gets an invisible text layer: you can search it and copy text from it.":
        "Le PDF reçoit une couche de texte invisible : vous pouvez y rechercher et copier "
        "du texte.",
    "The PDF contains the page images only.": "Le PDF ne contient que les images des pages.",
    "Languages in the document": "Langues du document",
    "Save": "Enregistrer",
    "Recognising text: page %1 of %2": "Reconnaissance du texte : page %1 sur %2",
    "Writing PDF: page %1 of %2": "Écriture du PDF : page %1 sur %2",
    "This takes a few seconds per page.": "Cela prend quelques secondes par page.",
    "English": "Anglais",
    "German": "Allemand",
    "French": "Français",
    "Spanish": "Espagnol",
    "Italian": "Italien",
    "The text recognition could not be started (%1).":
        "La reconnaissance du texte n'a pas pu démarrer (%1).",
    "The PDF file could not be created.": "Impossible de créer le fichier PDF.",
    "Page %1 could not be read.": "Impossible de lire la page %1.",
    "The text on page %1 could not be recognised.":
        "Le texte de la page %1 n'a pas pu être reconnu.",
    "The PDF file could not be written.": "Impossible d'écrire le fichier PDF.",
}

ES = {
    "Back": "Atrás",
    "Appearance": "Apariencia",
    "Like the phone": "Como el teléfono",
    "Light": "Claro",
    "Dark": "Oscuro",
    "Language": "Idioma",
    "Settings": "Ajustes",
    "Text recognition: Tesseract OCR. The languages for it are chosen when saving a PDF.":
        "Reconocimiento de texto: Tesseract OCR. Los idiomas se eligen al guardar un PDF.",
    # --- home -------------------------------------------------------------
    "%1 KB": "%1 KB",
    "%1 MB": "%1 MB",
    "Continue scan (%n page(s))": ["Continuar escaneo (%n página)",
                                   "Continuar escaneo (%n páginas)"],
    "New scan": "Nuevo escaneo",
    "Saved documents": "Documentos guardados",
    "No documents yet. Tap \"New scan\" to scan your first one.":
        "Aún no hay documentos. Toca «Nuevo escaneo» para escanear el primero.",
    "Open or share...": "Abrir o compartir...",
    "Open or share with": "Abrir o compartir con",
    "Rename": "Renombrar",
    "Delete": "Eliminar",
    "Rename document": "Renombrar documento",
    "A document with this name already exists.": "Ya existe un documento con este nombre.",
    "Delete document": "Eliminar documento",
    "Delete \"%1\"? This cannot be undone.": "¿Eliminar «%1»? No se puede deshacer.",
    # --- scanning ---------------------------------------------------------
    "New document": "Nuevo documento",
    "Discard": "Descartar",
    "Discard document": "Descartar documento",
    "Delete all scanned pages of this document?":
        "¿Eliminar todas las páginas escaneadas de este documento?",
    "Add pages with the camera or import a photo.":
        "Añade páginas con la cámara o importa una foto.",
    "Camera": "Cámara",
    "Import": "Importar",
    "Import a photo from": "Importar una foto desde",
    "Edit corners and filter": "Editar esquinas y filtro",
    "Rotate": "Girar",
    "Move earlier": "Mover antes",
    "Move later": "Mover después",
    "Delete page": "Eliminar página",
    "The photo could not be taken.": "No se pudo tomar la foto.",
    "The camera is not available: %1": "La cámara no está disponible: %1",
    "Hold the phone above the page": "Sostén el teléfono sobre la página",
    "Flash on": "Flash activado",
    "Flash off": "Flash desactivado",
    "Take photo": "Tomar foto",
    "Adjust page": "Ajustar página",
    "Edit page %1": "Editar página %1",
    "Whole photo": "Foto completa",
    "Use page": "Usar página",
    "Colour": "Color",
    "Grey": "Gris",
    "Black & white": "Blanco y negro",
    "Processing page...": "Procesando página...",
    "The photo could not be opened.": "No se pudo abrir la foto.",
    "The page could not be saved.": "No se pudo guardar la página.",
    # --- saving -----------------------------------------------------------
    "Save as PDF": "Guardar como PDF",
    "Saved \"%1\"": "«%1» guardado",
    "Title": "Título",
    "Scan %1": "Escaneo %1",
    "%n page(s)": ["%n página", "%n páginas"],
    "Recognise text (OCR)": "Reconocer texto (OCR)",
    "The PDF gets an invisible text layer: you can search it and copy text from it.":
        "El PDF recibe una capa de texto invisible: puedes buscar en él y copiar texto.",
    "The PDF contains the page images only.": "El PDF solo contiene las imágenes de las páginas.",
    "Languages in the document": "Idiomas del documento",
    "Save": "Guardar",
    "Recognising text: page %1 of %2": "Reconociendo texto: página %1 de %2",
    "Writing PDF: page %1 of %2": "Escribiendo PDF: página %1 de %2",
    "This takes a few seconds per page.": "Esto tarda unos segundos por página.",
    "English": "Inglés",
    "German": "Alemán",
    "French": "Francés",
    "Spanish": "Español",
    "Italian": "Italiano",
    "The text recognition could not be started (%1).":
        "No se pudo iniciar el reconocimiento de texto (%1).",
    "The PDF file could not be created.": "No se pudo crear el archivo PDF.",
    "Page %1 could not be read.": "No se pudo leer la página %1.",
    "The text on page %1 could not be recognised.":
        "No se pudo reconocer el texto de la página %1.",
    "The PDF file could not be written.": "No se pudo escribir el archivo PDF.",
}

IT = {
    "Back": "Indietro",
    "Appearance": "Aspetto",
    "Like the phone": "Come il telefono",
    "Light": "Chiaro",
    "Dark": "Scuro",
    "Language": "Lingua",
    "Settings": "Impostazioni",
    "Text recognition: Tesseract OCR. The languages for it are chosen when saving a PDF.":
        "Riconoscimento del testo: Tesseract OCR. Le lingue si scelgono al salvataggio di un PDF.",
    # --- home -------------------------------------------------------------
    "%1 KB": "%1 KB",
    "%1 MB": "%1 MB",
    "Continue scan (%n page(s))": ["Continua scansione (%n pagina)",
                                   "Continua scansione (%n pagine)"],
    "New scan": "Nuova scansione",
    "Saved documents": "Documenti salvati",
    "No documents yet. Tap \"New scan\" to scan your first one.":
        "Ancora nessun documento. Tocca «Nuova scansione» per scansionare il primo.",
    "Open or share...": "Apri o condividi...",
    "Open or share with": "Apri o condividi con",
    "Rename": "Rinomina",
    "Delete": "Elimina",
    "Rename document": "Rinomina documento",
    "A document with this name already exists.": "Esiste già un documento con questo nome.",
    "Delete document": "Elimina documento",
    "Delete \"%1\"? This cannot be undone.":
        "Eliminare «%1»? L'operazione non può essere annullata.",
    # --- scanning ---------------------------------------------------------
    "New document": "Nuovo documento",
    "Discard": "Scarta",
    "Discard document": "Scarta documento",
    "Delete all scanned pages of this document?":
        "Eliminare tutte le pagine scansionate di questo documento?",
    "Add pages with the camera or import a photo.":
        "Aggiungi pagine con la fotocamera o importa una foto.",
    "Camera": "Fotocamera",
    "Import": "Importa",
    "Import a photo from": "Importa una foto da",
    "Edit corners and filter": "Modifica angoli e filtro",
    "Rotate": "Ruota",
    "Move earlier": "Sposta prima",
    "Move later": "Sposta dopo",
    "Delete page": "Elimina pagina",
    "The photo could not be taken.": "Impossibile scattare la foto.",
    "The camera is not available: %1": "La fotocamera non è disponibile: %1",
    "Hold the phone above the page": "Tieni il telefono sopra la pagina",
    "Flash on": "Flash attivo",
    "Flash off": "Flash spento",
    "Take photo": "Scatta foto",
    "Adjust page": "Regola pagina",
    "Edit page %1": "Modifica pagina %1",
    "Whole photo": "Foto intera",
    "Use page": "Usa pagina",
    "Colour": "Colore",
    "Grey": "Grigio",
    "Black & white": "Bianco e nero",
    "Processing page...": "Elaborazione della pagina...",
    "The photo could not be opened.": "Impossibile aprire la foto.",
    "The page could not be saved.": "Impossibile salvare la pagina.",
    # --- saving -----------------------------------------------------------
    "Save as PDF": "Salva come PDF",
    "Saved \"%1\"": "«%1» salvato",
    "Title": "Titolo",
    "Scan %1": "Scansione %1",
    "%n page(s)": ["%n pagina", "%n pagine"],
    "Recognise text (OCR)": "Riconosci testo (OCR)",
    "The PDF gets an invisible text layer: you can search it and copy text from it.":
        "Il PDF riceve un livello di testo invisibile: puoi cercarvi e copiarne il testo.",
    "The PDF contains the page images only.": "Il PDF contiene solo le immagini delle pagine.",
    "Languages in the document": "Lingue del documento",
    "Save": "Salva",
    "Recognising text: page %1 of %2": "Riconoscimento del testo: pagina %1 di %2",
    "Writing PDF: page %1 of %2": "Scrittura del PDF: pagina %1 di %2",
    "This takes a few seconds per page.": "Ci vogliono alcuni secondi per pagina.",
    "English": "Inglese",
    "German": "Tedesco",
    "French": "Francese",
    "Spanish": "Spagnolo",
    "Italian": "Italiano",
    "The text recognition could not be started (%1).":
        "Impossibile avviare il riconoscimento del testo (%1).",
    "The PDF file could not be created.": "Impossibile creare il file PDF.",
    "Page %1 could not be read.": "Impossibile leggere la pagina %1.",
    "The text on page %1 could not be recognised.":
        "Impossibile riconoscere il testo della pagina %1.",
    "The PDF file could not be written.": "Impossibile scrivere il file PDF.",
}

# English is complete with its plural forms alone.
LANGUAGES = {"en": (EN, False), "de": (DE, True), "fr": (FR, True),
             "es": (ES, True), "it": (IT, True)}


def fill(message, value):
    translation = message.find("translation")
    if message.get("numerus") == "yes":
        if not isinstance(value, list):
            return False
        forms = translation.findall("numerusform")
        for i, form in enumerate(forms):
            form.text = value[min(i, len(value) - 1)]
    else:
        if isinstance(value, list):
            return False
        translation.text = value
    translation.attrib.pop("type", None)  # drop type="unfinished"
    return True


def translate(code, texts, complete):
    path = f"i18n/ubdocscan_{code}.ts"
    tree = ET.parse(path)
    missing = []
    for message in tree.iter("message"):
        source = message.find("source").text
        if source in texts:
            if not fill(message, texts[source]):
                missing.append(source + "  (plural form mismatch)")
        elif complete:
            missing.append(source)
    tree.write(path, encoding="utf-8", xml_declaration=True)
    return path, missing


failed = False
for code, (texts, complete) in LANGUAGES.items():
    path, missing = translate(code, texts, complete)
    if missing:
        failed = True
        print(f"{path}: missing translations:")
        for text in sorted(set(missing)):
            print("  ", repr(text))
    else:
        print(f"{path}: all texts translated")
sys.exit(1 if failed else 0)
