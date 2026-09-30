# Arkher Photoreal Demo (projeto de teste — M2)

Projeto pronto para testar o **pilar 1** ("fotorreal em qualquer celular")
no editor Arkher E no Godot estável 4.3+. É também um template: o addon
`arkher_quality` aqui dentro é o que você copiaria para os seus jogos.

## O que tem

| Cena | O que mostra |
|------|--------------|
| `demo_photoreal_1.tscn` | **Showcase** — golden hour, AgX, SDFGI, SSAO, SSIL, glow, fog, SSR, reflexos, objetos PBR + diretor com overlay de debug (qualidade máxima + resolução dinâmica) |
| `demo_a70.tscn` | **Gate do M2** — mesma cena com `startup_preset = profile_a70.tres` (calibração conservadora do Itel A70: max_scale 0.8, SDFGI/volumétrico/SSR off, sombras só med/high) |

## Teste no A70 em 5 passos (o gate do M2)

1. Baixe o artefato `arkher-linux-editor` do workflow **🏔 Arkher Builds**
   (Actions) e instale/extraia o editor Arkher no seu PC.
2. Abra este projeto no editor Arkher (File → Open Project).
3. Conecte o **Itel A70** via USB (USB debugging ligado, modo developer).
4. Cene raiz `demo_a70.tscn` como main (ou abra a cena e use o botão
   **one-click deploy** da barra — o preset "A70 (Android)" já vem com
   arm64 + certificado debug + one_click_deploy).
5. **Critério:** a demo segura **≥55fps** com a escala entre 0.4 e 0.8
   (720p nativo) sem engasgo. O overlay mostra fps/escala/tier ao vivo.

> Keystore: o editor Arkher gera o **debug keystore** automaticamente na
> primeira export — não precisa criar nada. Para publicar depois, crie um
> keystore próprio (Godot → Editor → Manage Export Templates/Android).

## Teste no topo (desktop / flagship)

Abra `demo_photoreal_1.tscn` → **Play**. Se quiser o perfil de topo:
arraste `addons/arkher_quality/presets/profile_top.tres` para
`startup_preset` do diretor.

## Sincronizar o addon

O `addons/arkher_quality/` aqui é uma **cópia** do que está em
`addons/arkher_quality/` na raiz do repo da engine (o res:// de um projeto
não enxerga fora do projeto). Quando o addon do repo mudar:

```sh
./sync_addon.sh
```

## Notas

- Renderer: **Forward+** (o pipeline fotorreal usa SDFGI/SSIL, que só
  existem no Forward+). No A70 o perfil desliga os efeitos caros — se o
  gate não passar, o M3 avalia variante com renderer mobile + GI probes.
- O diretor **auto-descobre** os `WorldEnvironment` (você não precisa
  apontar a propriedade `world_environment`).
- Presets: `addons/arkher_quality/presets/` (`profile_a70.tres`,
  `profile_top.tres`) — edite-os para calibrar por dispositivo.
