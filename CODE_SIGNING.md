# Politique de signature de code

Statut : **demande en préparation**. Les binaires Windows ne sont pas encore signés.

## Projet et périmètre

Nappe est un projet open source sous licence GPL-3.0-or-later, dépôt public. Les binaires Windows concernés sont `Nappe.exe` et la bibliothèque `Nappe.vst3` contenue dans le dossier du plugin.

## Processus de build

- Seuls les artefacts produits par la CI GitHub Actions, à partir du dépôt public, sont soumis à signature.
- Chaque demande de signature est approuvée manuellement par le mainteneur dans SignPath.
- Le dépôt n'accepte que des commits relus. Les modifications des workflows de release sont visibles dans l'historique.

## Accès et authentification

Tous les comptes ayant accès au dépôt GitHub ou au projet SignPath doivent utiliser l'authentification à deux facteurs.

## Vie privée

Nappe ne collecte aucune donnée personnelle et n'ouvre aucune connexion réseau.

## Mention obligatoire après approbation

Après approbation par SignPath Foundation, cette section devra contenir la mention exigée par le programme :

> Free code signing provided by SignPath.io, certificate by SignPath Foundation.

Dans les propriétés du fichier, Windows affichera alors SignPath Foundation comme éditeur, et non le nom du mainteneur.
