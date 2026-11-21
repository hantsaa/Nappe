# Politique de sécurité

## Signaler une vulnérabilité

Ne publie pas la faille dans une issue publique. Utilise l'onglet **Security → Report a vulnerability** du dépôt, qui ouvre un signalement privé. Le signalement reste confidentiel jusqu'à la publication d'un correctif.

## Versions supportées

Seule la dernière release publiée reçoit des correctifs de sécurité.

## Périmètre

Le plugin VST3 et l'application autonome. Ils n'ouvrent aucune connexion réseau et ne collectent aucune donnée personnelle. Ils ne lisent ni n'écrivent de fichier, à l'exception de l'état sauvegardé par l'hôte et des réglages audio de l'application autonome.

## Chaîne de confiance

- Les binaires sont compilés par GitHub Actions à partir du dépôt public.
- Les actions GitHub utilisées sont épinglées à un identifiant de commit précis.
- Chaque release publie un fichier `SHA256SUMS.txt` pour vérifier l'intégrité du zip.
- La signature Authenticode est en cours de demande auprès de SignPath Foundation (voir `CODE_SIGNING.md`). Tant qu'elle n'est pas active, les binaires ne sont pas signés.
